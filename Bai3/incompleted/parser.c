/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
// Da thuc hien 1+2+3+4+5
#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken; // token hien tai
Token *lookAhead; // token xem truoc

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  //printf("%s ",tmp);

  free(tmp);
}


/*void abc(void){
	compileExpress();
	eat(SB_ASSIGN);
	eat(KW_IF);
	compileConditon();
}*/


// eat de duyet cac ki hieu ket thuc
void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    //printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  //assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  //assert("Program parsed!");
}

void compileBlock(void) {
  //assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  //assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {
  //assert("Parsing subconstants...");

  while (lookAhead->tokenType == TK_IDENT){
    compileConstDecl();
  }
  //assert("Subconstants parsed!");
}

void compileConstDecl(void) {
  //assert("Parsing constant...");
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
  //assert("Constant parsed!");
}

void compileTypeDecls(void) {
  //assert("Parsing subtypes...");

  while (lookAhead->tokenType == TK_IDENT){
    compileTypeDecl();
  }
  //assert("Subtypes parsed!");
}

void compileTypeDecl(void) {
  //assert("Parsing a type...");
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
  //assert("Type parsed!");
}

void compileVarDecls(void) {
  while(lookAhead->tokenType == TK_IDENT){
    compileVarDecl();
  }
}

void compileVarDecl(void) {
  eat(TK_IDENT);
  while(lookAhead->tokenType==SB_COMMA){
    eat(SB_COMMA );
    eat(TK_IDENT );
  }
  //eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) {
  //assert("Parsing subtoutines ....");
  if (lookAhead->tokenType == KW_FUNCTION) {
      compileFuncDecl();
      compileSubDecls();
  } else if (lookAhead->tokenType == KW_PROCEDURE) {
      compileProcDecl();
      compileSubDecls();
  }
  //assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  //assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  //assert("Function parsed ....");
}

void compileProcDecl(void) {
  //assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  //assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
  if(lookAhead->tokenType == TK_IDENT){
    eat(TK_IDENT);
  }else if(lookAhead->tokenType == TK_NUMBER){
    eat(TK_NUMBER);
  }else if(lookAhead->tokenType == TK_CHAR){
    eat(TK_CHAR);
  }else{
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }

}

void compileConstant(void) {
  //assert("Parsing a constant...");
  if(lookAhead-> tokenType == TK_CHAR){
    eat(TK_CHAR);
  }else if(lookAhead-> tokenType == SB_PLUS){
    eat(SB_PLUS);
    compileConstant2();
  }else if(lookAhead-> tokenType == SB_MINUS){
    eat(SB_MINUS);
    compileConstant2();
  }else{
    compileConstant2();
  }
  //assert("Constant parsed!");
}

void compileConstant2(void) {
  if(lookAhead->tokenType == TK_IDENT )
    eat(TK_IDENT);
  else if(lookAhead->tokenType == TK_NUMBER)
    eat(TK_NUMBER);
  else
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
}

void compileType(void) {
  if(lookAhead->tokenType == TK_IDENT){
    eat(TK_IDENT);
  }else if(lookAhead->tokenType == KW_CHAR){
    eat(KW_CHAR);
  }else if(lookAhead->tokenType == KW_INTEGER){
    eat(KW_INTEGER);
  }else if(lookAhead->tokenType == KW_ARRAY){
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileType();
  }else{
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileBasicType(void) {
  if(lookAhead->tokenType == KW_CHAR)
    eat(KW_CHAR);
  else if(lookAhead->tokenType == KW_INTEGER)
    eat(KW_INTEGER);
  else
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
}

void compileParams(void) {
  if(lookAhead->tokenType==SB_LPAR){
    eat(SB_LPAR);
    if(lookAhead->tokenType==SB_RPAR){
      eat(SB_RPAR);
    }else{
      compileParam();
      compileParams2();
      eat(SB_RPAR);
    }
    
  }else if(lookAhead->tokenType==SB_COLON || lookAhead->tokenType == SB_SEMICOLON ){
    // follow(1) doc truoc 1 ki tu de xu li
    // gap colon(:) va semicolon(;) thi ko thuc hien gi
  }else{
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParams2(void) {
  if(lookAhead->tokenType== SB_SEMICOLON){
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }else if(lookAhead->tokenType== SB_RPAR){
    // follow(1) doc truoc 1 ki tu
  }else{
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParam(void) {
  if(lookAhead->tokenType== KW_VAR){
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }else if(lookAhead->tokenType== TK_IDENT){
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }else{
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileStatements(void) {
  compileStatement();
  compileStatements2();
  // if(lookAhead->tokenType!= SB_SEMICOLON){
  //     error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
  //   }
}

void compileStatements2(void) {
  // if(lookAhead->tokenType== SB_SEMICOLON){
  //   eat(SB_SEMICOLON);
  //   compileStatement();
  //   compileStatements2();
  //   //eat(SB_SEMICOLON);
  // }else if(lookAhead->tokenType == KW_END){
  //   // doc truoc 1 ki tu (KW_END)
  // }else{
  //   error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
  // }
  if(lookAhead->tokenType== SB_SEMICOLON){
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  }else if(lookAhead->tokenType == KW_END){
    // doc truoc 1 ki tu (KW_END)
  }else{
    //error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    eat(SB_SEMICOLON);
  }
}

//FIRST(Statement) = {TK_IDENT, KW_CALL, KW_BEGIN, KW_IF, KW_WHILE,KW_FOR, e}
//FOLLOW(Statement) = {SB_SEMICOLON, KW_END, KW_ELSE}
void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // doc truoc 1 ki tu de ktra, neu dung thi tiep tuc, sai thi bao loi
  case SB_SEMICOLON:// ;
  case KW_END:
  case KW_ELSE:
    break;
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void) {
  //assert("Parsing an assign statement ....");
  
  eat(TK_IDENT);
  if(lookAhead->tokenType == SB_COMMA){
	while(1){
		eat(SB_COMMA);
		eat(TK_IDENT);
		if(lookAhead->tokenType == SB_ASSIGN)
			break;
	}
}


  //compileExpression();
  if (lookAhead->tokenType == SB_LSEL) {
      compileIndexes();
  }
  eat(SB_ASSIGN);


  compileExpression();
	if(lookAhead->tokenType == SB_COMMA){
		while(1){
			eat(SB_COMMA);
			compileExpression();
			
			if(lookAhead->tokenType == SB_SEMICOLON)
				break;
			
		}
	} 
  //assert("Assign statement parsed ....");
}



void compileABC(void) {
	compileExpression();
	eat(SB_ASSIGN);
	if (lookAhead->tokenType == KW_IF) {
      		eat(KW_IF);
		compileCondition();
		eat(KW_THEN);
		compileExpression();
		eat(KW_ELSE);
		compileExpression();
  	}else{
  		compileExpression();
	}
}


void compileCallSt(void) {
  //assert("Parsing a CALL statement ....");
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  //assert("CALL statement parsed ....");
}

void compileGroupSt(void) {
  //assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  //assert("Group statement parsed ....");
}

void compileIfSt(void) {
  //assert("Parsing an IF statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  //assert("IF statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  //assert("Parsing a WHILE statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  //assert("WHILE statement pased ....");
}

void compileForSt(void) {
  //assert("Parsing a FOR statement ....");
  eat(KW_FOR);
  if(lookAhead->tokenType== SB_LPAR){

    eat(SB_LPAR);
    compileAssignSt();
    eat(SB_SEMICOLON);
    compileCondition();
    eat(SB_SEMICOLON);
    compileAssignSt();
    eat(SB_RPAR);
  }else{

    eat(TK_IDENT);
    eat(SB_ASSIGN);
    compileExpression();
    eat(KW_TO);
    compileExpression();
    eat(KW_DO);
  }
  
  compileStatement();
  //assert("FOR statement parsed ....");
}

// Follow(Arguments) ={SB_SEMICOLON, KW_END, KW_ELSE, SB_COMMA, SB_EQ, SB_NEQ, SB_LE, SB_LT,
// SB_GE, SB_GT, SB_TIMES, SB_SLASH, KW_TO, KW_DO, SB_RSEL, SB_RPAR, KW_THEN }
void compileArguments(void) {
  // switch (lookAhead->tokenType) {
  // case SB_LPAR:
  //     eat(SB_LPAR);
  //     compileExpression();
  //     compileArguments2();
  //     eat(SB_RPAR);
  //     break;
  // // doc truoc 1 ki tu neu khac cac truong hop duoi thi se bao loi
  // // Token Follow cua statement
  // case SB_SEMICOLON:// token ;
  // case KW_END:
  // case KW_ELSE:

  // // Token Follow cua arguments2
  // case SB_COMMA: // token ,

  // // Token Follow cua condition2
  // case SB_EQ: // token =
  // case SB_NEQ: // token !=
  // case SB_LE: // token <=
  // case SB_LT: // token <
  // case SB_GE: // token >=
  // case SB_GT: // token >

  // // Token Follow cua term2
  // case SB_TIMES: // token *
  // case SB_SLASH: // token /

  // // Token Follow cua expression3
  // case KW_TO:
  // case KW_DO:
  
  // // Token Follow cua indexes
  // case SB_RSEL:

  // // Token Follow cua factor
  // case SB_RPAR:
  
  // // Token Follow cua (if statement)
  // case KW_THEN:
  //     break;
  // default:
  //     error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  //     break;
  // }


  switch (lookAhead->tokenType) {
  case SB_LPAR:
      eat(SB_LPAR);
      if(lookAhead->tokenType== SB_RPAR ){
        eat(SB_RPAR);
      }else{
        compileExpression();
        compileArguments2();
        eat(SB_RPAR);
      }
      
      break;
  default:
      error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileArguments2(void) {
  if(lookAhead->tokenType == SB_COMMA){
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  }else if(lookAhead->tokenType == SB_RPAR ){
    // Token follow cua conditon
  }else{
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void) {
  compileExpression();
  compileCondition2();
}

// First(condition2) = { SB_EQ, SB_NEQ, SB_LE, SB_LT, SB_GE, SB_GT}
void compileCondition2(void) {
  switch (lookAhead->tokenType) {
  case SB_EQ://token =
      eat(SB_EQ);
      compileExpression();
      break;
  case SB_NEQ: // token !=
      eat(SB_NEQ);
      compileExpression();
      break;
  case SB_LE: // token <=
      eat(SB_LE);
      compileExpression();
      break;
  case SB_LT: // token <
      eat(SB_LT);
      compileExpression();
      break;
  case SB_GE: // token >=
      eat(SB_GE);
      compileExpression();
      break;
  case SB_GT: // token >
      eat(SB_GT);
      compileExpression();
      break;
  default:
      error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileExpression(void) {
  //assert("Parsing an expression");
  if(lookAhead->tokenType== SB_PLUS ){
    eat(SB_PLUS);
    compileExpression2();
  }else if(lookAhead->tokenType == SB_MINUS ){
    eat(SB_MINUS);
    compileExpression2();
  }else{
    compileExpression2();
  }
  //assert("Expression parsed");
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  switch(lookAhead->tokenType) {
  case SB_MINUS:
      eat(SB_MINUS);
      compileTerm();
      compileExpression3();
      break;

  case SB_PLUS:
      eat(SB_PLUS);
      compileTerm();
      compileExpression3();
      break;
  //Follow cua Expression3
  // doc truoc 1 ki tu, neu sai se bao loi
  // Token Follow cua arguments2
  case SB_COMMA:

  // Token Follow cua condition2
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:

  // Token Follow cua statement
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:

  // Token Follow cua (For statement)
  case KW_TO:
  case KW_DO:

  // Token Follow cua indexes
  case SB_RSEL:
  
  // Token Follow cua factor
  case SB_RPAR:

  // Token Follow cua (if statement)
  case KW_THEN:
      break;
  default:
      error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  switch (lookAhead->tokenType) {
  case SB_TIMES:
      eat(SB_TIMES);
      compileFactor();
      compileTerm2();
      break;
  case SB_SLASH:
      eat(SB_SLASH);
      compileFactor();
      compileTerm2();
      break;
  // Follow cua Term2
  // doc truoc 1 ki tu, neu khac case ben duoi thi bao loi
  // (For statement)
  case KW_TO:
  case KW_DO:

  // condition2
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:

  // statement
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:

  // expression3
  case SB_PLUS:
  case SB_MINUS:
  
  
  // arguments2
  case SB_COMMA:
  
  // factor
  case SB_RPAR:

  // indexes
  case SB_RSEL:

  // (if statement)
  case KW_THEN:
      break;
  default:
      error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileFactor(void) {
  if(lookAhead->tokenType==TK_CHAR || lookAhead->tokenType== TK_NUMBER ){
    compileUnsignedConstant();
  }else if(lookAhead->tokenType==SB_LPAR ){
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
  }else if(lookAhead->tokenType== TK_IDENT){
    eat(TK_IDENT);
    if(lookAhead->tokenType== SB_LSEL )
      compileIndexes();
    else if(lookAhead->tokenType==SB_LPAR )
      compileArguments();
  }else{
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileIndexes(void) {
  if (lookAhead->tokenType == SB_LSEL) {
      eat(SB_LSEL);
      compileExpression();
      eat(SB_RSEL);
      compileIndexes();
  }
}

int compile(char *FileName) {
  if (openInputStream(FileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
