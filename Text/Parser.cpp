#include "Text/Parser.h" 
#include "Common/Common.h" 
#include "Rendering/ObjectViewer.h" 
#include <set> 

using namespace Str; 

Token Token::COMMA = {SPECIAL,SYM_COMMA,0,0,""}; 
Token Token::PLUS = {SPECIAL,SYM_PLUS,0,0,""}; 
Token Token::MINUS = {SPECIAL,SYM_MINUS,0,0,""}; 
Token Token::DIV = {SPECIAL,SYM_DIV,0,0,""}; 
Token Token::MULT = {SPECIAL,SYM_MULT,0,0,""}; 
Token Token::LPAREN = {SPECIAL,SYM_LPAREN,0,0,""}; 
Token Token::RPAREN = {SPECIAL,SYM_RPAREN,0,0,""}; 
Token Token::LBRACK = {SPECIAL,SYM_LBRACK,0,0,""}; 
Token Token::RBRACK = {SPECIAL,SYM_RBRACK,0,0,""}; 
Token Token::EQ = {SPECIAL,SYM_EQ,0,0,""};
Token Token::TEND = {END,SYM_NULL,0,0,""};  

set<string> Parser::_reserved;

#define PARSER_DEBUG 0
#define INPT_DEBUG 1

Token Tokenizer::getToken(bool peeking){
	int end = _end; 

	Token ret; 
	if(_pos>end)
		return Token::TEND; 
	ret.type = FAILED; 

	if(_str[end]=='=')      {ret = Token::EQ; end--; }
	else if(_str[end]=='+') {ret = Token::PLUS; end--; }
	else if(_str[end]=='-') {ret = Token::MINUS; end--; }
	else if(_str[end]=='*') {ret = Token::MULT; end--; }
	else if(_str[end]=='(') {ret = Token::LPAREN; end--; }
	else if(_str[end]==')') {ret = Token::RPAREN; end--; }
	else if(_str[end]=='[') {ret = Token::LBRACK; end--; }
	else if(_str[end]==']') {ret = Token::RBRACK; end--; }
	else if(_str[end]=='/') {ret = Token::DIV; end--; }
	else if(_str[end]==',') {ret = Token::COMMA; end--; }
	else if(isLetter(_str[end])||isNumber(_str[end])||_str[end]=='.'){
		string ns; 
		bool beSymbol = false; 
		bool beNumber = false; 
		int dotCount = 0; 
		for(int j=end;j>=_pos;j--){
			if(isLetter(_str[j])||isNumber(_str[j])||_str[end]=='.'){
				ns=_str[j]+ns;
				if(isLetter(_str[j])){
					beSymbol=true;
				}
				else if(_str[end]=='.')
					dotCount++; 
				else
					beNumber=true;
			}
			else
				break;
			end--; 
		}

		if(isLetter(ns[0])){
			if(dotCount==0){
				ret.type = SYMBOL; 
				ret.sval = ns; 
			}
		}
		else if(isNumber(ns[0])||ns[0]=='.'){
			if(dotCount<=1 && !beSymbol && beNumber){
				if(dotCount>0){
					ret.type = NUMBERD; 
					ret.dval = Str::parseDouble(ns); 
				}
				else{
					ret.type = NUMBERI; 
					ret.ival = Str::parseInt(ns); 
				}
			}
		}
	}

	if(!peeking)
		_end = end; 
	return ret; 
}

// cleans an input string: removes white spaces, new lines, tabs.  
string Parser::clean(const string& s){
	string ret="";
	for(unsigned int j=0;j<s.length();j++){
		if(s[j]==' '||s[j]=='\n'||s[j]=='\t')
			continue; 
		ret+=s[j]; 
	}
	return ret; 
}

string Parser::trim(const string& s){
	string ret="";
	string ret2="";
	bool clean = true; 
	for(unsigned int j=0;j<s.length();j++){
		if(clean && (s[j]==' '||s[j]=='\n'||s[j]=='\t'))
			continue; 
		else
			clean = false; 
		ret+=s[j]; 
	}

	clean = true; 
	for(int j=ret.length()-1;j>=0;j--){
		if(clean && (ret[j]==' '||ret[j]=='\n'||ret[j]=='\t'))
			continue; 
		else
			clean = false; 
		ret2=ret[j]+ret2; 
	}

	return ret2; 
}


void Parser::setup(){
	// language commands
	_reserved.insert("Eq"); 
	_reserved.insert("For"); 
	_reserved.insert("Block"); 
	_reserved.insert("Hold"); 
	_reserved.insert("Pi"); 
	_reserved.insert("Cos"); 
	_reserved.insert("Sin"); 
	_reserved.insert("Pos"); 
	_reserved.insert("Int"); 
	_reserved.insert("Print"); 

	// turtle commands
	_reserved.insert("Forward"); 
	_reserved.insert("Move"); 
	_reserved.insert("Turn"); 
	_reserved.insert("Resize"); 
	_reserved.insert("Color"); 
	_reserved.insert("Linesize"); 
	_reserved.insert("ResetTurtle"); 
	_reserved.insert("ResetAll"); 
}

Atom* Parser::parse(const string& ins){
	setup(); 
	string s = clean(ins); 
	Tokenizer tk(s); 
	Atom* a = parseSt(&tk); 
	if(a!=NULL && tk.getPos()>tk.getEnd()){
		return a; 
	}
	else{
		cout<<"Parsing failed.  Remaining string: "<<tk.toString()<<endl;
		return NULL; 
	}
}

Atom* Parser::parseNumber(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseNumber: "<<tok->toString()<<endl;

	Token t = tok->getToken(); 
	Number* ret = NULL;
	if(t.type==NUMBERI || t.type==NUMBERD){
		ret = Number::value2number(&t); 
	}
	else{
		if(PARSER_DEBUG) cout<<"parseNumber: failed parsing number token."<<endl;
	}
	return ret; 
}

Atom* Parser::parseSymbol(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseSymbol: "<<tok->toString()<<endl;

	Token t = tok->getToken(); 
	Symbol* ret = NULL;
	if(t.type==SYMBOL){
		ret = new Symbol(); 
		ret->val = t.sval; 
	}
	else{
		if(PARSER_DEBUG) cout<<"parseSymbol: failed parsing number token."<<endl;
	}
	return ret; 
}

Atom* Parser::parseNumberOrSymbol(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseNumberOrSymbol: "<<tok->toString()<<endl;

	Token t = tok->peekToken(); 
	if(t.type==SYMBOL)
		return parseSymbol(tok); 
	else if(t.type==NUMBERD || t.type==NUMBERI)
		return parseNumber(tok); 
	return NULL; 
}

Atom* Parser::parseUnOpNumberOrSymbol(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseUnOpNumberOrSymbol: "<<tok->toString()<<endl;

	Atom* a = parseNumberOrSymbol(tok); 
	if(a){
		Token t = tok->peekToken(); 
		if(t==Token::MINUS){
			tok->getToken(); 
			NegOp* ret = new NegOp(); 
			ret->addOperand(a); 
			return ret; 
		}
		else
			return a; 
	}
	else
		if(PARSER_DEBUG) cout<<"parseUnOpNumberOrSymbol: cannot parse number/symbol after -"<<endl;
	return NULL; 
}


Atom* Parser::parseUnOpNumberOrSymbolOrFuncInst(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseUnOpNumberOrSymbolOrFuncInst: "<<tok->toString()<<endl;
	Tokenizer prevtok = (*tok); 
	Atom* ret = parseFuncInst(tok); 
	if(ret!=NULL){
		return ret; 
	}
	else{
		(*tok) = prevtok; 
		ret = parseUnOpNumberOrSymbol(tok); 
	}
	if(ret==NULL)
		if(PARSER_DEBUG) cout<<"parseUnOpNumberOrSymbolOrFuncInst: failed on all three"<<endl;

	return ret; 
}


Atom* Parser::parseComp(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseComp: "<<tok->toString()<<endl;

	Atom* rp = parseFactor(tok); 
	if(rp!=NULL){
		Token t = tok->peekToken(); 
		if(t==Token::MULT || t==Token::DIV){
			t = tok->getToken(); 
			Atom* lp = parseComp(tok); 
			if(t==Token::MULT){
				MultOp* ret2 = new MultOp(); 
				ret2->addOperand(lp); 				
				ret2->addOperand(rp); 
				return ret2; 
			}
			else{
				DivOp* ret2 = new DivOp(); 
				ret2->addOperand(lp); 				
				ret2->addOperand(rp); 
				return ret2; 
			}
		}
		else{
			return rp; 
		}
	}
	if(PARSER_DEBUG) cout<<"parseComp: cannot parse factor"<<endl;
	return NULL; 
}

Atom* Parser::parseExp(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseExp: "<<tok->toString()<<endl;

	Atom* rp = parseComp(tok); 
	if(rp!=NULL){
		Token t = tok->peekToken(); 
		if(t==Token::PLUS|| t==Token::MINUS){
			t = tok->getToken(); 
			Atom* lp = parseExp(tok); 
			if(t==Token::PLUS){
				AddOp* ret2 = new AddOp(); 
				ret2->addOperand(lp); 				
				ret2->addOperand(rp); 
				return ret2; 
			}
			else{
				SubOp* ret2 = new SubOp(); 
				ret2->addOperand(lp); 				
				ret2->addOperand(rp); 
				return ret2; 
			}
		}
		else{
			NegOp* no = dynamic_cast<NegOp*>(rp); 
			MultOp* multo = dynamic_cast<MultOp*>(rp); 
			DivOp* divo = dynamic_cast<DivOp*>(rp); 

			int end = tok->getEnd(); 
			string str = tok->getStr(); 

			if((str.length()>(end+1) && str[end+1]=='-') && (no||multo||divo)){
				Atom* lp = parseExp(tok); 
				if(lp){
					AddOp* ret2 = new AddOp(); 
					ret2->addOperand(lp); 				
					ret2->addOperand(rp); 
					return ret2; 
				}
			}

			return rp; 
		}
	}
	if(PARSER_DEBUG) cout<<"parseExp: cannot parse comp"<<endl;

	return NULL; 
}

Atom* Parser::parseFactor(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseFactor: "<<tok->toString()<<endl;

	Token t = tok->peekToken(); 
	if(t==Token::RPAREN){
		tok->getToken(); 
		Atom* a = parseExp(tok); 
		t = tok->getToken(); 
		if(t!=Token::LPAREN){
			if(PARSER_DEBUG) cout<<"parseFactor: missing a left paren"<<endl;
			return NULL; 
		}
		else{
			return a; 
		}
	}
	else {
		Atom* lp = parseUnOpNumberOrSymbolOrFuncInst(tok); 
		if(lp!=NULL)
			return lp; 
		else{
			if(PARSER_DEBUG) cout<<"parseFactor: cannot parse number or symbol or funcinst"<<endl;
			return NULL; 
		}
	}
	if(PARSER_DEBUG) cout<<"parseFactor: cannot parse Factor"<<endl;

	return NULL; 
}

Atom* Parser::parseDef(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseDef: "<<tok->toString()<<endl;

	Atom* exp = parseExp(tok); 
	if(exp){
		Token t = tok->getToken(); 
		if(t==Token::EQ){

			Tokenizer prevtok = (*tok); 
			FuncOp* fop = (FuncOp*) parseFuncDef(tok); 
			if(fop){
				// variable aliasing to avoid scope issues in function definition
				map<string,string> alias; 
				for(unsigned int j=0;j<fop->paramList->list.size();j++){
					Symbol* s = (Symbol*) fop->paramList->list[j]; 
					alias[s->val] = s->val+fop->getUid(); 
					s->val = alias[s->val]; 
				}

				AliasVisitor avis(alias); 
				exp->accept(&avis,NULL); 
				FAsgOp* fasg = new FAsgOp(); 
				fasg->func = fop; 
				fasg->statement = exp; 
				return fasg; 
			}
			else{
				(*tok)=prevtok;
				Symbol* sym = (Symbol*) parseSymbol(tok); 
				if(sym && !reserved(sym->val)){
					AsgOp* asg = new AsgOp(); 
					asg->symbol = sym; 
					asg->statement = exp; 
					return asg; 
				}
				else{
					if(!sym)
						if(PARSER_DEBUG) cout<<"parseDef: missing symbol"<<endl;
					else
						if(PARSER_DEBUG) cout<<"parseDef: "<<sym->val<<" is reserved"<<endl;
				}
			}
		}
		else
			if(PARSER_DEBUG) cout<<"parseDef: missing EQ sign"<<endl;
	}
	else
		if(PARSER_DEBUG) cout<<"parseDef: missing statement"<<endl;

	return NULL; 
}

Atom* Parser::parseFuncInst(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseFuncInst: "<<tok->toString()<<endl;

	Token t = tok->getToken();  
	if(t==Token::RBRACK){
		List* plist = (List*) parseParamList(tok);
		if(plist){
			t = tok->getToken(); 
			if(t==Token::LBRACK){
				Symbol* ret = (Symbol*) parseSymbol(tok); 
				if(ret==NULL){
					if(PARSER_DEBUG) cout<<"parseFuncInst: cannot parse function name"<<endl;
					return NULL; 
				}

				if(ret->val=="Hold"){
					if(plist->list.size()==1){
						Hold* hold = new Hold(); 
						hold->statement = plist->list[0]; 
						delete plist; 
						delete ret;
						return hold; 
					}
					else
						if(PARSER_DEBUG) cout<<"parseFuncInst: wrong number of arguments for Hold"<<endl;
				}
				else if(ret->val=="For"){
					if(plist->list.size()==4){
						FuncOp* fop = new FuncOp(); 
						fop->name = ret; 
						fop->paramList = plist;

						map<string,string> alias; 
						Symbol* counter = (Symbol*) plist->list[0]; 
						alias[counter->val] = counter->val+fop->getUid(); 
						counter->val = alias[counter->val]; 
						AliasVisitor avis(alias); 
						plist->list[3]->accept(&avis,NULL); 

						return fop; 
					}
					else
						if(PARSER_DEBUG) cout<<"parseFuncInst: wrong number of arguments for For"<<endl;
				}
				else{
					FuncOp* fop = new FuncOp(); 
					fop->name = ret; 
					fop->paramList = plist;
					return fop; 
				}
			}
			else{
				delete plist; 
				if(PARSER_DEBUG) cout<<"parseFuncInst: missing lbracket"<<endl;
			}
		}
		else
			if(PARSER_DEBUG) cout<<"parseFuncInst: missing param list"<<endl;
	}
	else
		if(PARSER_DEBUG) cout<<"parseFuncInst: missing rbracket"<<endl;

	return NULL; 
}

Atom* Parser::parseSt(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseSt: "<<tok->toString()<<endl;

	Tokenizer pretok = (*tok); 
	Atom* ret = parseDef(tok); 
	if(ret==NULL){
		if(PARSER_DEBUG) cout<<"parseSt: cannot parse def"<<endl;

		(*tok) = pretok; 
		ret = parseExp(tok); 
	}

	if(ret==NULL)
		if(PARSER_DEBUG) cout<<"parseSt: cannot parse exp"<<endl;		

	return ret; 
}

Atom* Parser::parseFuncDef(Tokenizer* tok){
	Token t = tok->getToken(); 
	if(t==Token::RBRACK){
		List* list = (List*) parseSymbolList(tok); 
		if(list!=NULL){
			t = tok->getToken(); 
			if(t==Token::LBRACK){
				Symbol* ret = (Symbol*) parseSymbol(tok); 
				if(ret){
					FuncOp* op= new FuncOp(); 
					op->paramList = list; 
					op->name = ret; 
					return op; 
				}
				else
					if(PARSER_DEBUG) cout<<"parseFuncDef: missing symbol"<<endl;
			}
			else
				if(PARSER_DEBUG) cout<<"parseFuncDef: missing right bracket"<<endl;
		}
		else
			if(PARSER_DEBUG) cout<<"parseFuncDef: cannot parse param list"<<endl;
	}
	else
		if(PARSER_DEBUG) cout<<"parseFuncDef: missing right bracket"<<endl;

	return NULL; 
}

Atom* Parser::parseParamList(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"paramlist: "<<tok->toString()<<endl;
	Tokenizer prevtok = (*tok); 
	Atom* dst = parseSt(tok); 
	if(dst){
		Token t = tok->peekToken(); 
		if(t==Token::COMMA){
			tok->getToken(); 
			List* nlist = (List*) parseParamList(tok); 
			if(nlist && nlist->list.size()>0){
				List* list = new List(); 
				for(unsigned int j=0;j<nlist->list.size();j++)
					list->list.push_back(nlist->list[j]); 
				list->list.push_back(dst); 
				delete nlist; 
				return list; 
			}
			else{
				if(nlist) delete nlist; 
				if(PARSER_DEBUG) cout<<"parseParamList: failed after comma"<<endl;
				delete dst; 
				return NULL; 
			}
		}
		else{
			List* list = new List(); 
			list->list.push_back(dst); 
			return list; 
		}
	}
	else{
		(*tok) = prevtok; 
		// assume empty list
		return new List(); 
	}
	if(PARSER_DEBUG) cout<<"parseParamList: failed on st"<<endl;
	return NULL; 
}

Atom* Parser::parseSymbolList(Tokenizer* tok){
	if(PARSER_DEBUG) cout<<"parseSymbolList: "<<tok->toString()<<endl;

	Tokenizer pretok = (*tok); 
	Atom* first = parseSymbol(tok); 
	if(first==NULL){
		(*tok) = pretok; 
		return new List();
	}
	else{
		List* ret = new List(); 
		Token t = tok->peekToken(); 
		if(t==Token::COMMA){
			tok->getToken(); 
			List* nl= (List*) parseSymbolList(tok); 

			if(nl==NULL || nl->list.size()==0){
				if(PARSER_DEBUG) cout<<"parseSymbolList: failed on next list"<<endl;
				delete ret; 
				if(nl) 
					delete nl; 
				ret = NULL; 
			}
			else{
				for(unsigned int j=0;j<nl->list.size();j++)
					ret->list.push_back(nl->list[j]); 
				ret->list.push_back(first); 
				delete nl; 
			}
			return ret; 
		}
		ret->list.push_back(first); 
		return ret; 
	}
	if(PARSER_DEBUG) cout<<"parseSymbolList: failed list"<<endl;

	return NULL; 
}


void EvalVisitor::visit(Number* op, void* ret){
	Value* v = (Value*) ret; 
	Value* nv = Number::number2value(op); 
	(*v) = (*nv); 
	delete nv; 
}

void EvalVisitor::visit(Symbol* op, void* ret){
	Value* v = (Value*) ret; 
	SymbolsTable::iterator it = _symtab->find(op->val); 
	Atom* a = NULL; 
	if(it==_symtab->end()){
		SymbolsTable* fstack = _funcstack->topFunc(); 
		if(fstack){
			it = fstack->find(op->val); 
			if(it==fstack->end()){
				v->type = FAILED; 
				if(INPT_DEBUG) cout<<"visit(Symbol): undefined variable "<<op->val<<endl;
				return; 
			}
			else
				a = it->second; 
		}
		else{
			v->type = FAILED; 
			if(INPT_DEBUG) cout<<"visit(Symbol): undefined variable "<<op->val<<endl;
			return; 
		}
	}
	else
		a = it->second; 

	if(a!=NULL){
		Number* num = dynamic_cast<Number*>(a); 
		if(num!=NULL){
			Token* t = Number::number2value(num); 
			(*v) = (*t); 
			delete t; 
		}
		else{
			Value nv; 		
			a->accept(this,&nv); 
			if(nv.type==NUMBERI || nv.type==NUMBERD){
				Number* num = Number::value2number(&nv); 
				(*_symtab)[op->val]=num; 
			}
			(*v)=nv; 
		}
	}
	else{
		v->type = FAILED; 
		if(INPT_DEBUG) cout<<"visit(Symbol): why here? var: "<<op->val<<endl;
	}
}

void EvalVisitor::visit(NegOp* op, void* ret){
	Value* v = (Value*) ret; 
	Atom* a = op->getOperand(0); 

	if(a==NULL){
		v->type = FAILED; 
		if(INPT_DEBUG) cout<<"negation failed"<<endl;
	}
	else{
		Value nv; 
		a->accept(this,&nv); 

		if(nv.type==NUMBERI){ 
			v->type = NUMBERI; 
			v->ival = -nv.ival; 
		}
		else if(nv.type==NUMBERD){
			v->type = NUMBERD; 
			v->dval = -nv.dval; 
		}
		else{
			v->type = FAILED; 
			if(INPT_DEBUG) cout<<"negation failed"<<endl;
		}
	}
}


void EvalVisitor::visit(MultOp* op, void* ret){
	Value* v = (Value*) ret; 

	Atom* l = op->getOperand(0); 
	Atom* r = op->getOperand(1); 

	if(l==NULL||r==NULL){
		v->type = FAILED; 
		if(INPT_DEBUG) cout<<"multiplication failed"<<endl;
	}
	else{
		Value lv,rv; 
		l->accept(this,&lv); 
		r->accept(this,&rv); 

		if(lv.type==NUMBERI && rv.type==NUMBERI){
			v->type = NUMBERI; 
			v->ival = lv.ival*rv.ival; 
		}
		else if((lv.type==NUMBERD && rv.type==NUMBERI) || 
			(lv.type==NUMBERI && rv.type==NUMBERD) || 
			(lv.type==NUMBERD && rv.type==NUMBERD)){

			if(lv.type==NUMBERI) lv.dval=lv.ival; 
			if(rv.type==NUMBERI) rv.dval=rv.ival; 

			v->type = NUMBERD; 
			v->dval = lv.dval*rv.dval; 
		}
		else{
			v->type = FAILED; 
			if(INPT_DEBUG) cout<<"multiplication failed"<<endl;
		}
	}
}

void EvalVisitor::visit(AddOp* op, void* ret){
	Value* v = (Value*) ret; 

	Atom* l = op->getOperand(0); 
	Atom* r = op->getOperand(1); 

	if(l==NULL||r==NULL){
		v->type = FAILED; 
		if(INPT_DEBUG) cout<<"addition failed"<<endl;
	}
	else{
		Value lv,rv; 
		l->accept(this,&lv); 
		r->accept(this,&rv); 

		if(lv.type==NUMBERI && rv.type==NUMBERI){
			v->type = NUMBERI; 
			v->ival = lv.ival+rv.ival; 
		}
		else if((lv.type==NUMBERD && rv.type==NUMBERI) || 
			(lv.type==NUMBERI && rv.type==NUMBERD) || 
			(lv.type==NUMBERD && rv.type==NUMBERD)){

			if(lv.type==NUMBERI) lv.dval=lv.ival; 
			if(rv.type==NUMBERI) rv.dval=rv.ival; 

			v->type = NUMBERD; 
			v->dval = lv.dval+rv.dval; 
		}
		else{
			v->type = FAILED; 
			if(INPT_DEBUG) cout<<"addition failed"<<endl;
		}
	}
}


void EvalVisitor::visit(SubOp* op, void* ret){
	Value* v = (Value*) ret; 

	Atom* l = op->getOperand(0); 
	Atom* r = op->getOperand(1); 

	if(l==NULL||r==NULL){
		v->type = FAILED; 
		if(INPT_DEBUG) cout<<"subtraction failed"<<endl;
	}
	else{
		Value lv,rv; 
		l->accept(this,&lv); 
		r->accept(this,&rv); 

		if(lv.type==NUMBERI && rv.type==NUMBERI){
			v->type = NUMBERI; 
			v->ival = lv.ival-rv.ival; 
		}
		else if((lv.type==NUMBERD && rv.type==NUMBERI) || 
			(lv.type==NUMBERI && rv.type==NUMBERD) || 
			(lv.type==NUMBERD && rv.type==NUMBERD)){

			if(lv.type==NUMBERI) lv.dval=lv.ival; 
			if(rv.type==NUMBERI) rv.dval=rv.ival; 

			v->type = NUMBERD; 
			v->dval = lv.dval-rv.dval; 
		}
		else{
			v->type = FAILED; 
			if(INPT_DEBUG) cout<<"subtraction failed"<<endl;
		}
	}
}

void EvalVisitor::visit(DivOp* op, void* ret){
	Value* v = (Value*) ret; 

	Atom* l = op->getOperand(0); 
	Atom* r = op->getOperand(1); 

	if(l==NULL||r==NULL){
		v->type = FAILED; 
		if(INPT_DEBUG) cout<<"addition failed"<<endl;
	}
	else{
		Value lv,rv; 
		l->accept(this,&lv); 
		r->accept(this,&rv); 

		if(lv.type==NUMBERI && rv.type==NUMBERI){
			if(rv.ival!=0){
				v->type = NUMBERI; 
				v->ival = lv.ival/rv.ival; 
			}
			else{
				v->type = FAILED; 
				if(INPT_DEBUG) cout<<"division by 0"<<endl;
			}
		}
		else if((lv.type==NUMBERD && rv.type==NUMBERI) || 
			(lv.type==NUMBERI && rv.type==NUMBERD) || 
			(lv.type==NUMBERD && rv.type==NUMBERD)){


			if(lv.type==NUMBERI) lv.dval=lv.ival; 
			if(rv.type==NUMBERI) rv.dval=rv.ival; 

			if(rv.dval!=0){
				v->type = NUMBERD; 
				v->dval = lv.dval/rv.dval; 
			}
			else{
				v->type = FAILED; 
				cout<<"division by 0"<<endl;
			}		
		}
		else{
			v->type = FAILED; 
			if(INPT_DEBUG) cout<<"addition failed"<<endl;
		}
	}
}


void EvalVisitor::visit(FAsgOp* op, void* ret){
	Value* v = (Value*) ret; 
	(*_symtab)[op->func->name->val] = op->func; 	
	op->func->statement = op->statement; 
	v->type = EVAL; 
} 

void EvalVisitor::visit(AsgOp* op, void* ret){
	Value* v = (Value*) ret; 
	Value tv; 
	op->statement->accept(this,&tv); 
	if(dynamic_cast<Number*>(op->statement)==NULL && (tv.type==NUMBERI||tv.type==NUMBERD)){
		Number* num = Number::value2number(&tv); 
		(*_symtab)[op->symbol->val] = num; 
		v->type = EVAL;
	}
	else{
		Symbol* s = op->symbol; 
		FindSymbolVisitor fsv(s);
		bool found = false;  
		op->statement->accept(&fsv,&found); 

		if(!found && tv.type!=FAILED){
			(*_symtab)[s->val] = op->statement; 
			v->type = EVAL;
		}
		else{
			if(tv.type!=FAILED){
				// the symbol appeared on both side of the assignment.  
				v->type = FAILED; 
				if(INPT_DEBUG) cout<<"visit(AsgOp): invalid recursive variable "<<s->val<<endl;
			}
			else{
				if(INPT_DEBUG) cout<<"visit(AsgOp): failed in evaluating right hand side "<<op->statement->toString()<<endl;
			}
		}
	}
}


// instf is the instantiation of a function (calling a function)
void EvalVisitor::visit(FuncOp* instf, void* ret){
	bool spec = Interpreter::specialFunc(this,instf,ret); 

	if(!spec){
		Value* outv = (Value*) ret; 
		SymbolsTable::iterator si = _symtab->find(instf->name->val); 
		if(si!=_symtab->end()){
			FuncOp* func = dynamic_cast<FuncOp*>(si->second); 
			if(func && func->paramList->list.size()==instf->paramList->list.size()){
				SymbolsTable ntab; 
				EvalHoldVisitor ehv(this); 
				for(unsigned int j=0;j<func->paramList->list.size();j++){
					Value curv; 
					Symbol* sym = (Symbol*) func->paramList->list[j]; 
					Atom* st = (Atom*) instf->paramList->list[j]; 
					st->accept(&ehv,&curv); 
					if(curv.type==NUMBERI || curv.type==NUMBERD)
						st = Number::value2number(&curv); 
					else if(curv.type==HOLD){
						// not quite sure about the logic here
						if(dynamic_cast<Symbol*>(st)!=NULL && _funcstack->topFunc())
							st = (*_funcstack->topFunc())[sym->val]; 
					}
					ntab[sym->val] = st; 
				}
				// leaking

				SymbolsTable* tab = _funcstack->newFunc(); 
				if(tab){
					(*tab) = ntab; 

					Value v; 
					func->statement->accept(this,&v); 
					(*outv) = v; 				

					_funcstack->popFunc(); 
				}
				else{
					cout<<"visit(FuncOp): exceeded function stack size."<<endl;
					outv->type = FAILED; 
				}
			}
			else{
				if(func)
					if(INPT_DEBUG) cout<<"visit(FuncOp): different number of parameters between inst and def"<<endl;
				else
					if(INPT_DEBUG) cout<<"visit(FuncOp): "<<instf->name->val<<" is not stored as a function"<<endl;

				outv->type = FAILED; 
			}
		}
		else{
			if(INPT_DEBUG) cout<<"visit(FuncOp): no entry "<<instf->name->val<<" in the symbols table"<<endl;			
			outv->type = FAILED; 
		}
	}
}

void EvalVisitor::visit(Hold* op, void* ret){
	op->statement->accept(this,ret); 
}

void EvalHoldVisitor::visit(Hold* op, void* ret){
	((Value*)ret)->type = HOLD; 
}

bool Interpreter::specialFunc(EvalVisitor* ev, FuncOp* instf, void* ret){
	string fname = instf->name->val; 
	Value* retv = (Value*) ret; 
	if(fname=="Block"){
		// to ensure block scoping, I assume this is slow
		SymbolsTable osym = *ev->_symtab; 

		List* plist = instf->paramList; 
		Value nv; 			
		for(unsigned int j=0;j<plist->list.size();j++){
			plist->list[j]->accept(ev,&nv); 
			if(nv.type==FAILED)
				break; 			
		}
		(*retv) = nv; 

		// ensure scope 
		for(SymbolsTable::iterator i=ev->_symtab->begin();i!=ev->_symtab->end();i++){
			if(osym.find(i->first)!=osym.end())
				osym[i->first] = i->second; 
		}

		(*ev->_symtab) = osym; 
		return true; 
	}
	else if(fname=="For"){
		List* plist = instf->paramList; 
		if(plist->list.size()==4){
			Symbol* counter = dynamic_cast<Symbol*>(plist->list[0]); 
			if(counter!=NULL && 
				ev->_symtab->find(counter->val)==ev->_symtab->end()){

				Value numst,numed; 
				plist->list[1]->accept(ev,&numst); 
				plist->list[2]->accept(ev,&numed); 

				if(numst.type==NUMBERI && numed.type==NUMBERI && (numst.ival<=numed.ival)){
					Number cn; 
					cn.isInt = true; 

					bool popped = false; 
					SymbolsTable* tab = ev->_funcstack->topFunc(); 
					if(!tab) {
						tab = ev->_funcstack->newFunc(); 
						popped = true; 
					}
					if(tab){
						Value nv; 
						(*tab)[counter->val] = &cn; 
						for(int j=numst.ival;j<=numed.ival;j++){
							cn.ival = j; 
							plist->list[3]->accept(ev,&nv); 
							if(nv.type==FAILED)
								break; 
						}

						tab->erase(counter->val); 
						(*retv) = nv; 
						if(popped)
							ev->_funcstack->popFunc(); 
					}
					else{
						if(INPT_DEBUG) cout<<"specialFunc(For): function stack size exceeded  "<<endl;
						retv->type = FAILED; 
					}

			}
				else{
					retv->type = FAILED; 
					if(INPT_DEBUG) cout<<"specialFunc(For): range is given incorrectly "<<numst.ival<<" "<<numed.ival<<endl;
				}
			}
			else {
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(For): first argument must be an undefined symbol "<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(For): wrong number of arguments"<<endl;
		}

		return true; 
	}
	else if(fname=="Eq"){
		List* plist = instf->paramList; 
		if(plist->list.size()==4){
			Value numl,numr; 
			plist->list[0]->accept(ev,&numl); 
			plist->list[1]->accept(ev,&numr); 

			if((numr.type==NUMBERI && numl.type==NUMBERI && (numr.ival==numl.ival))||
				(numr.type==NUMBERD && numl.type==NUMBERD && (numr.dval==numl.dval))){
					plist->list[2]->accept(ev,retv); 
			}
			else
				plist->list[3]->accept(ev,retv); 
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Eq): wrong number of arguments"<<endl;
		}

		return true; 
	}
	else if(fname=="Cos" || fname=="Sin"){
		List* plist = instf->paramList; 
		if(plist->list.size()==1){
			Value num; 
			plist->list[0]->accept(ev,&num); 

			if(num.type==NUMBERI || num.type==NUMBERD){
				double ang = num.type==NUMBERI ? num.ival : num.dval; 
				retv->type = NUMBERD; 
				double ret = fname=="Cos" ? cos(ang) : sin(ang); 
				if(abs(ret)<1.e-9)
					ret = 0; 
				retv->dval = ret; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Cos/Sin): argument does not evaluate to a number"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Cos/Sin): wrong number of arguments"<<endl;
		}

		return true; 
	}
	else if(fname=="Move"){
		List* plist = instf->paramList; 
		if(plist->list.size()==1){
			Value num; 
			plist->list[0]->accept(ev,&num); 
			if(num.type==NUMBERI || num.type==NUMBERD){
				if(num.type==NUMBERI)
					ev->_turtlem->move((double)num.ival); 
				else
					ev->_turtlem->move(num.dval); 
				retv->type = EVAL; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Move): wrong arguments type"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Move): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="Forward"){
		List* plist = instf->paramList; 
		if(plist->list.size()==1){
			Value num; 
			plist->list[0]->accept(ev,&num); 
			if(num.type==NUMBERI || num.type==NUMBERD){
				if(num.type==NUMBERI)
					ev->_turtlem->forward((double)num.ival); 
				else
					ev->_turtlem->forward(num.dval); 
				retv->type = EVAL; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Forward): wrong argument type"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Forward): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="Turn"){
		List* plist = instf->paramList; 
		if(plist->list.size()==1){
			Value num; 
			plist->list[0]->accept(ev,&num); 
			if(num.type==NUMBERI || num.type==NUMBERD){
				if(num.type==NUMBERI)
					ev->_turtlem->turn((double)num.ival); 
				else
					ev->_turtlem->turn(num.dval); 
				retv->type = EVAL; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Turn): arguments type"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Turn): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="Resize"){
		List* plist = instf->paramList; 
		if(plist->list.size()==1){
			Value num; 
			plist->list[0]->accept(ev,&num); 
			if(num.type==NUMBERI || num.type==NUMBERD){
				if(num.type==NUMBERI)
					ev->_turtlem->resize((double)num.ival); 
				else
					ev->_turtlem->resize(num.dval); 
				retv->type = EVAL; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Resize): arguments type"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Resize): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="Color"){
		List* plist = instf->paramList; 
		if(plist->list.size()==3){
			Value r,g,b;
			plist->list[0]->accept(ev,&r); 
			plist->list[1]->accept(ev,&g); 
			plist->list[2]->accept(ev,&b); 
			if((r.type==NUMBERI || r.type==NUMBERD) && 
				(g.type==NUMBERI || g.type==NUMBERD) && 
				(b.type==NUMBERI || b.type==NUMBERD)){
				double dr = r.type==NUMBERI ? r.ival : r.dval; 
				double dg = g.type==NUMBERI ? g.ival : g.dval; 
				double db = b.type==NUMBERI ? b.ival : b.dval; 
				ev->_turtlem->color(dr,dg,db); 
				retv->type = EVAL; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Color): arguments type"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Color): wrong number of arguments"<<endl;
		}

		return true; 
	}
	else if(fname=="Linesize"){
		List* plist = instf->paramList; 
		if(plist->list.size()==1){
			Value num; 
			plist->list[0]->accept(ev,&num); 
			if(num.type==NUMBERI || num.type==NUMBERD){
				if(num.type==NUMBERI)
					ev->_turtlem->lineSize(num.ival); 
				else
					ev->_turtlem->lineSize(num.dval); 
				retv->type = EVAL; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Linesize): arguments type"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Linesize): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="ResetTurtle"){
		List* plist = instf->paramList; 
		if(plist->list.size()==0){
			ev->_turtlem->reset(); 
			retv->type = EVAL; 
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Reset): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="ResetAll"){
		List* plist = instf->paramList; 
		if(plist->list.size()==0){
			ev->_turtlem->reset(); 
			(*ev->_symtab) = SymbolsTable(); 
			ev->_funcstack->clear(); 
			ev->_stHistory->clear(); 
			retv->type = EVAL; 
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(ResetAll): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="Pos"){
		List* plist = instf->paramList; 
		if(plist->list.size()==0){
			Turtle::Pt p = ev->_turtlem->pos(); 
			cout<<"p: ("<<p.x<<","<<p.y<<")"<<endl;
			retv->type = EVAL; 
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Pos): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="Int"){
		List* plist = instf->paramList; 
		if(plist->list.size()==1){
			Value num; 
			plist->list[0]->accept(ev,&num); 

			if(num.type==NUMBERI || num.type==NUMBERD){
				int nval = num.type==NUMBERI ? num.ival : (int) num.dval; 
				retv->type = NUMBERI; 
				retv->ival = nval; 
			}
			else{
				retv->type = FAILED; 
				if(INPT_DEBUG) cout<<"specialFunc(Cos/Sin): argument does not evaluate to a number"<<endl;
			}
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Int): wrong number of arguments"<<endl;
		}
		return true; 
	}
	else if(fname=="Print"){
		List* plist = instf->paramList; 
		if(plist->list.size()>=1){
			Value num; 

			for(int j=0;j<plist->list.size();j++){
				if(dynamic_cast<Hold*>(plist->list[j])){
					cout<<"Hold"; 
				}
				else{
					plist->list[j]->accept(ev,&num); 

					if(dynamic_cast<Symbol*>(plist->list[j])&&(num.type==NUMBERI||num.type==NUMBERD))
						cout<<(dynamic_cast<Symbol*>(plist->list[j]))->val<<"=";

					if(num.type==NUMBERI)
						cout<<num.ival; 
					else if(num.type==NUMBERD)
						cout<<num.dval;
					else if(num.type==EVAL)
						cout<<"Evaluation";
					else
						cout<<"Failed";
				}
				if(j<plist->list.size()-1)
					cout<<", "; 
			}
			cout<<endl;
			retv->type = EVAL; 
		}
		else{
			retv->type = FAILED; 
			if(INPT_DEBUG) cout<<"specialFunc(Print): wrong number of arguments"<<endl;
		}
		return true; 
	}

	return false; 
}

pair<bool,string> Interpreter::interpret(const string& s){
	EvalVisitor ev(&_symtab,&_fstack,&_turtlem,&_stHistory); 
	Atom* ast = Parser::parse(s); // build the abstract syntax 
	pair<bool,string> ret; 
	if(ast){
		Value v; 
		ast->accept(&ev,&v); 
		// leaking
		// need to figure out a way to delete this without danger? 
		if(v.type!=FAILED){
			ret.first = true; 
			if(v.type==NUMBERI)
				ret.second = Str::toString(v.ival); 
			else if(v.type==NUMBERD)
				ret.second = Str::toString(v.dval); 
			else if(v.type==EVAL)
				ret.second = "Evaulated"; 
			if(s!="ResetAll[]")
				_stHistory.push_back(s); 
			if(_viewer) 
				_viewer->redraw(); 
		}
		else{
			ret.first = false; 
			ret.second = "Evaluation failed"; 
		}
	}
	else{
		ret.first = false; 
		ret.second = "Parsing failed - Syntax Error"; 
	}

	return ret; 
}