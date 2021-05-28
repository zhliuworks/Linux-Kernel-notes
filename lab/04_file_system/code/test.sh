genromfs -V "vromfs" -f lab4.img -d src
insmod romfs.ko hided_file_name=aa encrypted_file_name=bb exec_file_name=ft
mount -o loop lab4.img tgt
ls -l tgt
umount tgt
rmmod romfs