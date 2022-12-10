from nltk.tree import *

# assign your output (generalied list of the syntax tree) to varaible text
text = "[P[D[int][V[Var[b]][,][V[Var[c]]]][;]][S[Sing[Ass[Var[b]][=][Exp[AS[MD[Fact[Const[3]]]][+][AS[MD[Fact[(][Exp[AS[MD[Fact[Const[4]]]]][>][Exp[AS[MD[Fact[Const[5]]]]]]][)]][*][MD[Fact[Const[6]]]]]]][==][Exp[AS[MD[Fact[Const[1]]]]]]]]]]]"


text = text.replace("(", "ob")    #in the syntax tree, 'ob' will display in place of '('
text = text.replace(")", "cb")    #in the syntax tree, 'cb' will display in place of ')'
text = text.replace("[", "(")
text = text.replace("]", ")")


tree = Tree.fromstring(text)
tree.pretty_print(unicodelines=True, nodedist=10)   

