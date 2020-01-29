#ifndef PARSER_H
#define PARSER_H

#include <iostream> 
#include <string> 
#include <vector>
#include <map> 
#include <set>
#include <list> 
#include "Common/Common.h" 
#include "Rendering/Turtle.h" 

using namespace std; 



enum SymbolType {SYM_NULL,SYM_COMMA,SYM_LPAREN,SYM_RPAREN,SYM_EQ,SYM_DIV,SYM_MULT,SYM_PLUS,SYM_MINUS,SYM_LBRACK,SYM_RBRACK}; 
enum TokenType {SYMBOL,NUMBERI,NUMBERD,SPECIAL,END,FAILED,EVAL,HOLD}; 

class Token{

public: 
	static Token EQ; 
	static Token PLUS; 
	static Token MINUS; 
	static Token DIV; 
	static Token MULT; 
	static Token LPAREN; 
	static Token RPAREN; 
	static Token LBRACK; 
	static Token RBRACK; 
	static Token COMMA;
	static Token TEND;  
public: 
	int type; 
	SymbolType special; 
	double dval; 
	int ival; 
	string sval; 

	bool operator==(const Token& rhs) const{
		return type==rhs.type &&
			special==rhs.special && 
			dval==rhs.dval &&
			ival==rhs.ival &&
			sval==rhs.sval; 
	}

	bool operator!=(const Token& rhs) const{
		return !((*this)==rhs); 
	}
}; 

// doubling token as a value.  They're not really the same thing, conceptually.
typedef Token Value; 

class Tokenizer{
protected: 
	string _str; 
	int _pos; 
	int _end; 

	Token getToken(bool peeking); 

public: 
	Tokenizer(const string& s) { 
		_str = s;
		_pos = 0; 
		_end = s.length()-1; 
	}

	Token getToken() { 
		return getToken(false); 
	}

	Token peekToken() { 
		return getToken(true); 
	}

	int getPos() { return _pos; } 
	int getEnd() { return _end; }
	string getStr() { return _str; } 
	string toString() { if(_pos<_str.length()) return _str.substr(_pos,_end+1); return ""; }
}; 

class AtomVisitor; 

class Atom{
public: 
	virtual void accept(AtomVisitor* visitor, void* ret)=0; 
	virtual string toString()=0; 
};

class Number; 
class Symbol; 
class NegOp; 
class MultOp; 
class AddOp; 
class SubOp; 
class DivOp; 
class FuncOp; 
class List; 
class FAsgOp; 
class AsgOp; 
class Hold; // box the evaluation

class AtomVisitor{
public: 
	virtual void visit(List* op, void* ret)=0; 
	virtual void visit(Number* op, void* ret)=0; 
	virtual void visit(Symbol* op, void* ret)=0; 
	virtual void visit(NegOp* op, void* ret)=0; 
	virtual void visit(MultOp* op, void* ret)=0; 
	virtual void visit(AddOp* op, void* ret)=0; 
	virtual void visit(SubOp* op, void* ret)=0; 
	virtual void visit(DivOp* op, void* ret)=0; 
	virtual void visit(FuncOp* op, void* ret)=0; 
	virtual void visit(FAsgOp* op, void* ret)=0;
	virtual void visit(AsgOp* op, void* ret)=0;
	virtual void visit(Hold* op, void* ret)=0; 
}; 

class List : public Atom{
public: 
	vector<Atom*> list; 

	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
	virtual string toString() { 
		string s="["; 
		for(unsigned int j=0;j<list.size();j++){
			if(j<list.size()-1)
				s+=list[j]->toString()+", "; 
			else
				s+=list[j]->toString();				
		}	
		s+="]"; 
		return s; 
	}
};

class Number : public Atom{
public: 
	bool isInt; 
	int ival; 
	double dval; 
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
	virtual string toString() { 
		if(isInt) return Str::toString(ival); return Str::toString(dval); }

	Number(){}

	Number(int i){
		isInt = true; 
		ival = i; 
	}

	Number(double d){
		isInt = false; 
		dval = d; 
	}

	static Number* value2number(Value* v){
		Number* n = new Number(); 
		if(v->type==NUMBERI||v->type==NUMBERD){
			if(v->type==NUMBERI){
				n->isInt = true; 
				n->ival = v->ival; 
			}
			else{
				n->isInt = false; 
				n->dval = v->dval; 
			}
			return n; 
		}
		return NULL; 
	}

	static Token* number2value(Number* n){
		Token* t = new Token(); 
		if(n->isInt){
			t->type = NUMBERI; 
			t->ival = n->ival; 
		}
		else{
			t->type = NUMBERD; 
			t->dval = n->dval; 
		}
		return t; 
	}
};

class Symbol : public Atom{
public: 
	string val; 
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
	virtual string toString() { return val; }
};

class Op : public Atom{
protected: 
	//vector<Atom*> _operands; 
	Atom* _operands[2]; 
	int _size; 
public: 
	Op() { _size = 0; }

	void addOperand(Atom* a) { _operands[_size++]=a; }
	int numOperands() { return _size; }
	Atom* getOperand(int i) { 
		if(i<_size) return _operands[i]; 
		return NULL; 
	}

	virtual string mychar()=0; 
	virtual string toString() { 
		string res=""; 
		for(unsigned int j=0;j<_size;j++)
			res+=_operands[j]->toString()+" "; 
		return "("+mychar()+" "+res+")"; 
	}
}; 

class NegOp : public Op{
public: 
	virtual string mychar() { return "-"; }
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
};

class MultOp : public Op{
public: 
	virtual string mychar() { return "*"; }
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
};

class AddOp : public Op{
public: 
	virtual string mychar() { return "+"; }
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
};

class SubOp : public Op{
public: 
	virtual string mychar() { return "-"; }
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
};

class DivOp : public Op{
public: 
	virtual string mychar() { return "/"; }
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
};

class FuncOp : public Atom{
protected: 
	string _uid; 
public: 
	FuncOp(){
		_uid = Str::toString(rand()%1000000); 
		isRec = false; 
	}

	bool isRec; // is it recursive
	Symbol* name; 
	List* paramList; 
	Atom* statement; // unevaluated statements
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
	virtual string toString() { 
		return name->toString()+paramList->toString(); 
	}
	string getUid() { return _uid; }
};

class Hold : public Atom{
public: 
	Atom* statement; 
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
	virtual string toString() { return string("Hold[[")+statement->toString()+"]]"; }
}; 

class FAsgOp : public Atom{
public: 
	FuncOp* func; 
	Atom* statement; 
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }

	virtual string toString() { 	
		return func->toString() + "={" + statement->toString()+"}"; 
	}
} ;

class AsgOp : public Atom{
public: 
	Symbol* symbol; 
	Atom* statement; 
	virtual void accept(AtomVisitor* visitor, void* ret){ visitor->visit(this, ret); }
	virtual string toString() { 	
		return symbol->toString() + "={" + statement->toString()+"}"; 
	}
} ;

//typedef map<string,Atom*> SymbolsTable; 

class SymbolsTable : public map<string,Atom*>{
public: 

	SymbolsTable(){
//		(*this)["Null"]=new Number(0); 
		(*this)["Pi"]=new Number(M_PI); 
	}

};

class EmptyVisitor : public AtomVisitor{
public: 
	virtual void visit(List* op, void* ret){}
	virtual void visit(Number* op, void* ret){}
	virtual void visit(Symbol* op, void* ret){}
	virtual void visit(NegOp* op, void* ret){}
	virtual void visit(MultOp* op, void* ret){}
	virtual void visit(AddOp* op, void* ret){}
	virtual void visit(SubOp* op, void* ret){}
	virtual void visit(DivOp* op, void* ret){}
	virtual void visit(FuncOp* op, void* ret){} 
	virtual void visit(FAsgOp* op, void* ret){}
	virtual void visit(AsgOp* op, void* ret){}
	virtual void visit(Hold* op, void* ret){}
};

// does not allow passing function as argument
class FunctionStack{
protected: 
	list<SymbolsTable*> _sstack; 
public: 
	SymbolsTable* newFunc() { 
		if(_sstack.size()<100){
			SymbolsTable* st = new SymbolsTable(); 
			_sstack.push_back(st); 
			return st; 
		}
		else
			return NULL; 
	}

	void popFunc() {
		if(_sstack.size()>0){
			SymbolsTable* st = _sstack.back(); 
			delete st; 
			_sstack.pop_back(); 
		}
	}

	SymbolsTable* topFunc(){
		if(_sstack.size()==0)
			return NULL; 
		return _sstack.back(); 
	}

	void clear(){
		for(list<SymbolsTable*>::iterator i=_sstack.begin();i!=_sstack.end();i++)
			delete *i; 
		_sstack.clear(); 
	}
}; 

class Interpreter; 
class EvalHoldVisitor; 

class EvalVisitor : public EmptyVisitor{
protected: 
	SymbolsTable* _symtab; 
	FunctionStack* _funcstack; 
	Turtle::TurtleMachine* _turtlem; 
	vector<string>* _stHistory; 
public: 
	EvalVisitor(SymbolsTable* st, FunctionStack* fs, Turtle::TurtleMachine* tm, vector<string>* sth) { 
		_symtab = st; 
		_funcstack = fs; 
		_turtlem = tm; 
		_stHistory = sth; 
	}
	virtual void visit(Number* op, void* ret); 
	virtual void visit(Symbol* op, void* ret); 
	virtual void visit(NegOp* op, void* ret); 
	virtual void visit(MultOp* op, void* ret); 
	virtual void visit(AddOp* op, void* ret); 
	virtual void visit(SubOp* op, void* ret); 
	virtual void visit(DivOp* op, void* ret); 
	virtual void visit(FuncOp* op, void* ret); 
	virtual void visit(FAsgOp* op, void* ret); 
	virtual void visit(AsgOp* op, void* ret); 
	virtual void visit(Hold* op, void* ret); 

	friend class Interpreter; 
	friend class EvalHoldVisitor; 
}; 

// for Hold ops, it will hold and not evaluate
class EvalHoldVisitor: public EvalVisitor{
public: 
	EvalHoldVisitor(EvalVisitor* ev): 
	  EvalVisitor(ev->_symtab,ev->_funcstack,ev->_turtlem,ev->_stHistory){}

	virtual void visit(Hold* op, void* ret); 
}; 


class TraverseTreeVisitor : public EmptyVisitor{
public: 
	virtual void visit(List* op, void* ret){
		for(unsigned int j=0;j<op->list.size();j++)
			op->list[j]->accept(this,ret); 
	}

	virtual void visit(NegOp* op, void* ret){
		op->getOperand(0)->accept(this,ret); 		
	}

	virtual void visit(MultOp* op, void* ret){
		op->getOperand(0)->accept(this,ret); 		
		op->getOperand(1)->accept(this,ret); 
	}

	virtual void visit(AddOp* op, void* ret){
		op->getOperand(0)->accept(this,ret); 		
		op->getOperand(1)->accept(this,ret); 
	}

	virtual void visit(SubOp* op, void* ret){
		op->getOperand(0)->accept(this,ret); 		
		op->getOperand(1)->accept(this,ret); 
	}

	virtual void visit(DivOp* op, void* ret){
		op->getOperand(0)->accept(this,ret); 		
		op->getOperand(1)->accept(this,ret); 
	}

	virtual void visit(FuncOp* op, void* ret){
		op->name->accept(this,ret); 
		op->paramList->accept(this,ret); 
	}

	virtual void visit(FAsgOp* op, void* ret){
		op->func->accept(this,ret); 
		op->statement->accept(this,ret); 
	}

	virtual void visit(AsgOp* op, void* ret){
		op->statement->accept(this,ret); 
		op->symbol->accept(this,ret); 
	}

	virtual void visit(Hold* op, void* ret){
		op->statement->accept(this,ret); 
	}
}; 


class AliasVisitor : public TraverseTreeVisitor{
protected: 
	map<string,string> _alias; 
public: 
	AliasVisitor(map<string,string> alias){
		_alias = alias; 
	}
	virtual void visit(Symbol* op, void* ret){
		if(_alias.find(op->val)!=_alias.end())
			op->val = _alias[op->val]; 
	}
}; 

class FindSymbolVisitor : public TraverseTreeVisitor{
protected: 
	Symbol* _sym; 

public: 
	FindSymbolVisitor(Symbol* s){ _sym = s; }
	virtual void visit(Symbol* op, void* ret){
		bool* bret = (bool*) ret; 
		if(_sym->val==op->val)
			(*bret) = true; 
	}
}; 

typedef pair<bool,bool> bpair; 
class FindFuncOpVisitor : public TraverseTreeVisitor{
protected: 
	FuncOp* _fop; 
public: 
	FindFuncOpVisitor(FuncOp* op){ _fop = op; }

	virtual void visit(FuncOp* op, void* ret){
		bpair* bret = (bpair*) ret; 
		cout<<"f: "<<op->name->val<<endl;
		bret->first = (op->name->val==_fop->name->val); 
		bret->second = (op->paramList->list.size()==_fop->paramList->list.size()); 
		op->paramList->accept(this,ret); 
	}
}; 

class Parser{
protected: 
	static set<string> _reserved; 
public: 
	static bool reserved(const string& s){
		return _reserved.find(s)!=_reserved.end(); 
	}

	static string clean(const string& s); 
	static string trim(const string& s); 
	static void setup(); 
	static Atom* parse(const string& s); 
	static Atom* parseNumber(Tokenizer* tok); 
	static Atom* parseSymbol(Tokenizer* tok); 
	static Atom* parseNumberOrSymbol(Tokenizer* tok); 
	static Atom* parseUnOpNumberOrSymbol(Tokenizer* tok); 
	static Atom* parseUnOpNumberOrSymbolOrFuncInst(Tokenizer* tok); 

	static Atom* parseExp(Tokenizer* tok); 
	static Atom* parseComp(Tokenizer* tok); 
	static Atom* parseFactor(Tokenizer* tok); 

	static Atom* parseDef(Tokenizer* tok); 
	static Atom* parseSt(Tokenizer* tok); 

	static Atom* parseFuncInst(Tokenizer* tok); 
	static Atom* parseFuncDef(Tokenizer* tok); 
	static Atom* parseParamList(Tokenizer* tok); 
	static Atom* parseSymbolList(Tokenizer* tok); 
}; 

class ObjectViewer; 

class Interpreter{
protected: 
	SymbolsTable _symtab; 
	FunctionStack _fstack; 
	Turtle::TurtleMachine _turtlem; 
	ObjectViewer* _viewer; 
	vector<string> _stHistory; 
public: 
	Interpreter() { 
		srand(12);
		_viewer = NULL; 
	}  // seed random for aliasing 

	pair<bool,string> interpret(const string& s); 
	static bool specialFunc(EvalVisitor* ev, FuncOp* f, void* ret); 

	void setViewer(ObjectViewer* ov) { _viewer = ov; }
	Turtle::TurtleMachine* getTurtleMachine() { return &_turtlem; }
	vector<string>* getHistory() { return &_stHistory; }; 
}; 

#endif