# Vita generic RTL enabler

## OwO what's that

TL;DR: allows to use pc usb-ethernet rtl8152 dongle on vita.

## Background

Vita Devkits and Testkits come with eth dongles (unless you bought it from eBay).  
Those adapters are hard to find and expensive. Thus we decided to dig into what they really are.  
There are two modules in bootimage: [usb_ether_smsc](https://wiki.henkaku.xyz/vita/SceUsbEtherSmsc) and [usb_ether_rtl](https://wiki.henkaku.xyz/vita/SceUsbEtherRtl)  
With the help of little RE and pop13_13 dongles we found out that they, along with custom sony VID/PID, support stock manufacturer VID/PIDs. And also conditions for them to load.  
RTL is the easiest one: it's used on PSTV, it's cheap and available. SMSC only works on non-CEX consoles, and there seems to be no ready to use dongles with it.  
So i ordered RTL one and started tinkering with code.  
  
Next problem: devkit/testkit adapters somehow enable host-mode on multiconnector. We probed a few things, S1ngyy even bought original sony dock, but that didn't help.  
Sooo... we just did it from software, thx to wiki and a little tinkering and RE.  
  
We have host mode on multiconn now, but obviously need a driver for device. You might say: "wait, but you said bootimage has them".  
Sure. But there's slight problem: RTL module only "starts" if conditions are met (see wiki), otherwise, module is still loaded but does nothing.  
We can't patch-out checks via taihen, since it's loaded way before taihen in boot_config.  
So, the easiest (and most cursed solution) that came to mind: patch-out dolce check in binary (`sceSblAimgrIsVITA() == 0` -> `sceSblAimgrIsVITA() == 1`), and patch module name (since rtl one is still loaded and you can't have two)  
While hackish, it is also safer method than hooking `sceSblAimgrIsVITA()`, because that way other parts of module and/or system software isn't affected.  
  
And finally, ethernet config in settings requires some capability flags, so we fake those too.

## Installation

### Retail
* copy ether-enabler.skprx to `ur0:tai`
* copy usb_ether_mtl.skprx to `ur0:tai`
* add both to `*KERNEL` section of config.txt, order shouldn't matter,

### Devkit/Testkit
* copy ether-enabler.skprx to `ur0:tai`
* add to `*KERNEL` section of config.txt

## Hardware

* usb ethernet with RTL8152.
* Make an adapter by connecting two usb-female port data pins, supply external power to one (dongle), or both (so vita would charge too), plug vita cable into another and letsgooo.
* Or buy Y-cable with two female ports.  

## Credits

* pop13_13 for initial idea and tinkering with devkit/testkit dongles.
* [CreepNT](https://github.com/CreepNT) for help with RE.
* [S1ngyy](https://github.com/S1ngyy) for measuring signals on multicon.
* [SKGleba](https://github.com/SKGleba) for testing some stuff.
* [Graphene](https://github.com/GrapheneCt) for answering stupid questions and help.
* [SonicMastr](https://github.com/SonicMastr) for his soft-bricked vita.
* And ofc everyone on CBPS and HENKAKU.