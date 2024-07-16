# CollectSysInfoUtil

Collects some information about the system and makes an arhive of it. Must be run under sudo.

Information to collect:
- OS, OS version, Kernel version
- List of installed packages
- journalctl for last 24 hours
- dmesg

## Build

```
make
```

## Run 
```
make run
```

## Clean temporary dir with data

(in case it was left by the prog due to some error)

```
make clean_data
```