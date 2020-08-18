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
./open
```

Press `Ctrl-C` on traceloop and search for the `openat` system calls:
```
00:00.005373292 cpu#3 pid 2264280 [write] openat(4294967196, "(Pointer deref failed!)", 2, 0) = 3
00:00.005430182 cpu#3 pid 2264280 [write] openat(4294967196, "/tmp/traceloop-test.data", 2, 0) = 4
```
The first `openat` always fails and the second always succeeds.

In case the systemd unit terminates with a non-zero status, you can reset it before running the next test:
```
sudo systemctl reset-failed
```

Restart the experiment but with `strace ./open`. This time, traceloop can successfully read the buffer.
```
00:12.803349204 cpu#3 pid 2264587 [strace] process_vm_readv(2264591, 140723332479072, 1, 140723332479088, 1, 0) = 4096
00:12.803378133 cpu#3 pid 2264587 [strace] write(2, "openat(AT_FDCWD, \"/tmp/traceloop-test.data\", O_RDWR", 51) = 51
00:12.803414606 cpu#3 pid 2264587 [strace] ptrace(24, 2264591, 0, 0) = 0
00:12.803474662 cpu#3 pid 2264587 [strace] wait4(4294967295, 140723332483692, 1073741824, 0)...
00:12.803643253 cpu#1 pid 2264591 [open] openat(4294967196, "/tmp/traceloop-test.data", 2, 0) = 3
...
00:12.804051970 cpu#1 pid 2264591 [open] openat(4294967196, "/tmp/traceloop-test.data", 2, 0) = 4
```

We notice that the `strace` process uses `process_vm_readv` on the pid of the
`open` process before the `openat` trace, so it warms the memory page
containing the filename.

## Test 2

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
