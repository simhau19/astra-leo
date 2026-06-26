# Using CSH with ASTRA-LEO

First of all you must download and install [CSH](https://github.com/spaceinventor/csh)

Now, setup your Ethernet interface in the [config.csh](./config.csh) file.
For Ethernet to work, you must give CSH permission to use raw Ethernet sockets.
Instructions for how to do this is given at runtime, but you could also just do it right away with

```bash
sudo setcap cap_net_raw+ep /path/to/csh
```

Now you can start CSH with

```bash
csh -i ./init.csh
```

you should now have two nodes defined; aleo-0 and aleo-1.
In CSH, set the node you want to communicate with using

```
node aleo-0
```

For development, you most likely only have one ASTRA-LEO connected to your Ethernet interface.
In that case `aleo-0` is probably the right one.

now you can get information about all the available parameters using

```
list download
```

Update all the variables by using

```
pull
```

Information about the rest of the available commands can be found in the [CSH Manual](https://github.com/spaceinventor/csh/tree/master/doc/MAN)
