from nltk.tree import *

# assign your output (generalied list of the syntax tree) to varaible text
text = "[P[D[int][V[Var[forr]][,][V[Var[b]][,][V[Var[c]][,][V[Var[howareyou]]]]]][;]][S[Sing[R[read][Var[b]][;]]][S[Sing[W[write][Var[b]][;]]][S[Sing[W[write][Const[5632]][;]]]]]]]"


text = text.replace("(", "ob")    #in the syntax tree, 'ob' will display in place of '('
text = text.replace(")", "cb")    #in the syntax tree, 'cb' will display in place of ')'
text = text.replace("[", "(")
text = text.replace("]", ")")


tree = Tree.fromstring(text)
tree.pretty_print(unicodelines=True, nodedist=10)   

