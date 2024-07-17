# CollectSysInfoUtil

Collects some information about the system and makes a gzip arhive of it. Must be run under sudo.

Information to collect:
- OS, OS version, Kernel version
- List of installed packages
- journalctl for last 24 hours
- dmesg
- Config files (if they don't exist, a warning is printed):
- - /etc/hosts
- - /etc/resolv.conf
- - /etc/ntp.conf
- - /etc/krb5.conf
- - /etc/samba/smb.conf
- - /etc/nsswitch.conf
- - /etc/NetworkManager/NetworkManager.conf

## Dependencies

### For usage

- libarchive

### For building

- libarchive-dev

## Build

```
make
```

## Run 
```
make run
```

## One of ways to extract files from resulting archive

```
mkdir out
tar -x -f sys_info.gz -C out/
```

## Clean dir with data

```
make clean_data
```