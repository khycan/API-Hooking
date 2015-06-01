##Python Version 3.4
import string

a = "g fmnc wms bgblr rpylqjyrc gr zw fylb. rfyrq ufyr amknsrcpq ypc dmp. bmgle gr gl zw fylb gq glcddgagclr ylb rfyr'q ufw rfgq rcvr gq qm jmle. sqgle qrpgle.kyicrpylq() gq pcamkkclbcb. lmu ynnjw ml rfc spj."

############ create translating table##############
table = str.maketrans(string.ascii_lowercase, string.ascii_lowercase[2:]+string.ascii_lowercase[:2])

b = a.translate(table)

print (b)
