# coding=utf-8
# Merge_Sort costs more than 10 times time than Bit_Sort!!!
import datetime

LEN = 10000
def merge_sort(lst1, lst2, start_index):
    len1 = len(lst1); len2 = len(lst2)
    i = 0; j = 0
    while True:
        if (i == len1) | (j == len2):  # 某个子序列遍历完成，跳出循环
            break
        if lst1[i] < lst2[j]:
            merged[start_index] = lst1[i]
            i += 1
            start_index += 1
        else:
            merged[start_index] = lst2[j]
            j += 1
            start_index += 1
    if i == len1:
        while j < len2:
            merged[start_index] = lst2[j]
            j += 1; start_index += 1
    elif j == len2:
        while i < len1:
            merged[start_index] = lst1[i]
            i += 1; start_index += 1

lst = [0]*LEN
for i in range(0, LEN):
    lst[i] = LEN-i

begin = datetime.datetime.now()
merged = [0]*len(lst)
sublen = 1
lstlen = len(lst)  # lstlen == 11
while sublen < lstlen:
    for i in range(0, lstlen, sublen*2):
        if (lstlen - i) <= sublen:
            while i < lstlen:  # just one sub-sequence left
                merged[i] = lst[i]
                i += 1
        elif (lstlen - i < 2 * sublen) & (lstlen - i > sublen):  # two sub-sequence left
            merge_sort(lst[i:i+sublen], lst[i+sublen:lstlen], i)
        else:
            merge_sort(lst[i:i+sublen], lst[i+sublen:i+sublen*2], i)
    sublen *= 2
    lst = merged
end = datetime.datetime.now()
print lst
print "It costs", end-begin, "seconds"