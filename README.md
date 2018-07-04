# [mac2mac](https://bluenix.pe/tools/mac2mac.html)
![version](https://img.shields.io/badge/version-1.0.0-blue.svg) ![license](https://img.shields.io/badge/license-MIT-green.svg)

mac2mac is an small and very fast tool that converts mac address format into all mac formats accepted. It uses a mini version of the OUI file provided by IEEE.

## Requirements
* GCC 4 or superior
* wget tool
* make

## Quick Start

1. git clone https://github.com/bluenix-pe/mac2mac.git
2. make
3. ./mac2mac -f test/mac.txt

## Examples
### Check a single MAC address

```console
$ ./mac2mac -s ecc4.0d04.a73e
ec-c4-0d-04-a7-3e   ec:c4:0d:04:a7:3e   ecc4.0d04.a73e   ecc4-0d04-a73e   ecc40d04a73e   EC-C4-0D Nintendo Co.,Ltd
```
### Check a file with MAC Address

```console
$ ./mac2mac -f test/mac.txt
HYPHEN              COLON               DOTTED           CDOTTED          NONE           OUI
ec-c4-0d-04-a7-3e   ec:c4:0d:04:a7:3e   ecc4.0d04.a73e   ecc4-0d04-a73e   ecc40d04a73e   EC-C4-0D Nintendo Co.,Ltd
00-24-73-ca-9c-da   00:24:73:ca:9c:da   0024.73ca.9cda   0024-73ca-9cda   002473ca9cda   00-24-73 3COM EUROPE LTD
00-24-7e-0d-3f-12   00:24:7e:0d:3f:12   0024.7e0d.3f12   0024-7e0d-3f12   00247e0d3f12   00-24-7E Universal Global Scientific Industrial Co., Ltd.
00-0c-29-5d-40-23   00:0c:29:5d:40:23   000c.295d.4023   000c-295d-4023   000c295d4023   00-0C-29 VMware, Inc.
1c-6f-65-03-c9-84   1c:6f:65:03:c9:84   1c6f.6503.c984   1c6f-6503-c984   1c6f6503c984   1C-6F-65 GIGA-BYTE TECHNOLOGY CO.,LTD.
1c-bd-b9-83-f9-c7   1c:bd:b9:83:f9:c7   1cbd.b983.f9c7   1cbd-b983-f9c7   1cbdb983f9c7   1C-BD-B9 D-Link International
2c-44-fd-1b-80-11   2c:44:fd:1b:80:11   2c44.fd1b.8011   2c44-fd1b-8011   2c44fd1b8011   2C-44-FD Hewlett Packard
40-01-c6-77-11-22   40:01:c6:77:11:22   4001.c677.1122   4001-c677-1122   4001c6771122   40-01-C6 3COM EUROPE LTD
44-37-e6-6c-26-4b   44:37:e6:6c:26:4b   4437.e66c.264b   4437-e66c-264b   4437e66c264b   44-37-E6 Hon Hai Precision Ind. Co.,Ltd.
68-7f-74-da-75-01   68:7f:74:da:75:01   687f.74da.7501   687f-74da-7501   687f74da7501   68-7F-74 Cisco-Linksys, LLC
```

### Convert result to Mayus
```console
$ ./mac2mac -M -s ecc4.0d04.a73e
EC-C4-0D-04-A7-3E   EC:C4:0D:04:A7:3E   ECC4.0D04.A73E   ECC4-0D04-A73E   ECC40D04A73E   EC-C4-0D Nintendo Co.,Ltd
```

## How to update mini.oui.txt file
```console
$ ./make_mini_oui.sh
```