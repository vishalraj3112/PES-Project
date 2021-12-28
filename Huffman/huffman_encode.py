#******************************************************************************************
# @file    :   huffman_encode.py
# @brief   :   Code for generating huffman tree from an input file which is taken from Linux 
#              Syslog file.
# @author  :   Vishal Raj
# @date    :   December 11, 2021
# @version :   1.0
#
# @tools   :   python3
#
# @link    :   Code referred from https://www.programiz.com/dsa/huffman-coding.
#*****************************************************************************************

#open file in read mode
data_file = open("input2.txt","r")

#read whole file in string
data = data_file.read()

#close file
data_file.close()

#print the file data
#print(data)

#loading the file string
string = data

# Creating tree nodes
class NodeTree(object):

    def __init__(self, left=None, right=None):
        self.left = left
        self.right = right

    def children(self):
        return (self.left, self.right)

    def nodes(self):
        return (self.left, self.right)

    def __str__(self):
        return '%s_%s' % (self.left, self.right)


#*********************************************************************************
# * @function:   huffman_code_tree
# *
# * @brief   :   The main function implementing huffman coding.
# *
# * @param   :   The tree nodes, direction of growth(left or right), binString of
# *          :   of character codes.
# *
# * @return  :   dictionary of huffman tree nodes.
#**********************************************************************************
def huffman_code_tree(node, left=True, binString=''):
    if type(node) is str:
        return {node: binString}
    (l, r) = node.children()
    d = dict()
    d.update(huffman_code_tree(l, True, binString + '0'))
    d.update(huffman_code_tree(r, False, binString + '1'))
    return d


# Calculating frequency
freq = {}
for c in string:
    if c in freq:
        freq[c] += 1
    else:
        freq[c] = 1

freq = sorted(freq.items(), key=lambda x: x[1], reverse=True)

nodes = freq

no_of_nodes = len(nodes)

while len(nodes) > 1:
    (key1, c1) = nodes[-1]
    (key2, c2) = nodes[-2]
    nodes = nodes[:-2]
    node = NodeTree(key1, key2)
    nodes.append((node, c1 + c2))

    nodes = sorted(nodes, key=lambda x: x[1], reverse=True)

huffmanCode = huffman_code_tree(nodes[0][0])

lst = []
matrix = [[]]
matrix_bits = [[]]

matrix = [0 for i in range(128)]
matrix_bits = [0 for i in range(128)]

#variables needed for iteration.
i = 0
j = 0
bits_cnt = 0


#From this point, the C header file is generated.
print('#ifndef _HUFFMAN_H_')
print('#define _HUFFMAN_H_\n\n')
print('#include<stdint.h>')

print('typedef struct {\n\tuint8_t data;\n\tuint32_t ccode;\n\tint32_t no_bits;\n\tint32_t freq;\n} huffman_table_t;\n')

print('huffman_table_t huffman_table[] = {')

#parsing the string in binary
for x in huffmanCode:
    word = huffmanCode[x]
    lst = list(word)
    lst.reverse() #reversing for reading LSB first
    for y in lst:
        if y == '1':
            matrix[ord(x)] += pow(2,j) #store in ascii location
            bits_cnt = j + 1 
        j += 1
    matrix_bits[ord(x)] = bits_cnt
    i += 1
    j = 0
    bits_cnt = 0


array_bits = []

i = 0

#Code for generating the huffman tree Look-up table in the C header file. 
for x in range(128):
    print('\t{%d, 0x%x, %d},'% (x , matrix[x], matrix_bits[x]))

print('\t};')
print("\n#endif")

#[EOF]