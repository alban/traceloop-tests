# Traceloop tests

## Test 1

In the first terminal, start a shell in a new cgroup:
```
sudo systemd-run --pty --working-directory=$PWD --unit tracelooptest /bin/bash
```

In the second terminal, start traceloop on that cgroup:
```
sudo ./traceloop cgroups --dump-on-exit /sys/fs/cgroup/unified/system.slice/tracelooptest.service/
```

Back in the first terminal, start the test program:
```
./open_buffer_border
```

Press `Ctrl-C` on traceloop and search for the `openat` system calls:
```
00:03.007786572 cpu#1 pid 2257392 [open_buffer_bor] openat(4294967196, "/tmp/test1.txt", 0, 0) = -1 (no such file or directory)
00:03.007955591 cpu#1 pid 2257392 [open_buffer_bor] openat(4294967196, "(Pointer deref failed!)", 0, 0) = -1 (no such file or directory)
```

In case the systemd unit terminates with a non-zero status, you can reset it before running the next test:
```
sudo systemctl reset-failed
```

**Explanation:** the argument to the first `openat` can be printed
successfully. The argument to the second `openat` fails to be read with
`bpf_probe_read()` because it reads too many bytes and it reaches outside of
the mapped page.

**Solution:** use `bpf_probe_read_user_str()`.
