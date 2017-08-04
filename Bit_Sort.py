# coding=utf-8
# Merge_Sort costs more than 10 times time than Bit_Sort!!!
import datetime

LEN = 10000
lst = [0]*LEN
for i in range(0, LEN):
    lst[i] = LEN - i
print lst
begin = datetime.datetime.now()
s_lst = [0]*len(lst)
bit = [0]*(LEN+100)
for i in lst:
    bit[i] = 1
    index = 0
for i in range(0, len(bit)):
    if bit[i] is not 0:
        s_lst[index] = i
        index += 1
end = datetime.datetime.now()
print s_lst
print "It costs", end-begin, "seconds"