# Huffman Coding in python

#open file in read mode
data_file = open("input2.txt","r")

#read whole file in string
data = data_file.read()

#close file
data_file.close()

#print the file data
#print(data)

#string = 'BCAADDDCCACACAC'
string = "geeksforgeeks"

#overlapping the small string
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


# Main function implementing huffman coding
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

#print("length:%d\n"%len(nodes))

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

#matrix = [[0 for i in range(10)] for i in range(20)]#row, Col
#matrix = [0 for i in range(no_of_nodes)]
#matrix_bits = [0 for i in range(no_of_nodes)]
matrix = [0 for i in range(128)]
matrix_bits = [0 for i in range(128)]

i = 0
j = 0
bits_cnt = 0

print('#ifndef _HUFFMAN_H_')
print('#define _HUFFMAN_H_\n\n')
print('#include<stdint.h>')

print('typedef struct {\n\tuint8_t data;\n\tuint32_t ccode;\n\tint32_t no_bits;\n\tint32_t freq;\n} huffman_table_t;\n')

print('huffman_table_t huffman_table[] = {')

#parsing the string in binary
for x in huffmanCode:
    word = huffmanCode[x]
    lst = list(word)
    #print(lst)
    lst.reverse() #reversing for reading LSB first
    for y in lst:
        if y == '1':
            matrix[ord(x)] += pow(2,j) #store in ascii location
            bits_cnt = j + 1 
        #else: 
            #print('0')
        j += 1
    #print("mat:")
    #print(matrix)
    #print("\n")
    matrix_bits[ord(x)] = bits_cnt
    i += 1
    j = 0
    bits_cnt = 0

#print("Matrix")
#print(matrix)

#print(word)
#print(lst)

array_bits = []

#matrix.reverse()
#huffmanCode.reverse()
i = 0
#for (char, frequency) in freq:
#->for x in huffmanCode:
#    print('{%-4r, 0x%x, %d},' % (x, int(matrix[i]), matrix_bits[i]))
#    i += 1
for x in range(128):
    print('\t{%d, 0x%x, %d},'% (x , matrix[x], matrix_bits[x]))

print('\t};')
print("\n#endif")