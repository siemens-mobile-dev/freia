@a166 %1.a66
@l166 %1.obj TO %1.abs PURGE
@oh166 %1.abs FLASH(0)
@hex2bin %1.h86 %1.bin
@del %1.h86
@del %1.abs
@del %1.obj
@del %1.m66
@del %1.lst
