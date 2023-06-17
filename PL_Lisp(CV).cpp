
# include <iostream>
# include <stdio.h>
# include <string>
# include <vector>
# include <sstream>
# include <exception>
# include <queue>


using namespace std;

/*
OurScheme Project Grammar

<S-EXP> ::= <ATOM> | LEFT_PAREN <S-EXP> {<S-EXP>} [ DOT <S-EXP> ] RIGHT_PAREN | QUOTE <S-EXP>

<ATOM> ::= SYMBOL | INT | FLOAT | STRING | NIL | T | LEFT_PAREN_RIGHT_PAREN

The concept of Project 1 consists of two main parts: Parser and Scanner.

Parser: Responsible for building the parse tree based on the grammar rules.

Scanner: Responsible for tokenizing the input for the Parser (the general principle is to Peek before getting it).

External functionalities provided:

GetToken: Returns the current token.
PeekToken: Peeks at the next valid token.
Internal functionalities:
(1) Reads the input until a valid token is encountered. Constructs a class structure for that token, including attributes like Token_ID, Token_TYPE, Line_Num, and Line_Column. Informs the Parser about the token.

(2) The method for determining a valid token is as follows:
- Reads the first character of the input and determines the token format based on the first character.
- Peeks at the next character. If it is not in the format of the current token, the current token is confirmed.
- If the next character is in the format of the current token, read the character and repeat the previous step

  OurScheme Project 文法

  <S-EXP> ::= <ATOM> | LEFT_PAREN <S-EXP> {<S-EXP>} [ DOT <S-EXP> ] RIGHT_PAREN | QUOTE <S-EXP>

  <ATOM> ::= SYMBOL | INT | FLOAT | STRING | NIL | T | LEFT_PAREN_RIGHT_PAREN


  大概概念係Project 有兩大部分: Parser 和 Scanner
  
  Parser :  負責根據文法樹建文法樹
            

  Scanner : 負責根據文法切Token給Parser (大原則為先Peek後Get)
            
            提供對外功能: GetToken : 回傳現時的Token
                          PeekToken : 偷看下一個有效的Token

            內部要做的功能 : (1) 把input讀進直到是有效Token, 把該Token建成1個class structure
                                 (包括 string Token_ID; string Token_TYPE ; int Line_Num, int Line_Column ;)
                                 ,並告訴Parser

                             (2) 確定有效Token的方法是先讀第一個input字元,根據First char決定是甚麼Token的格式,之後先偷看下個字元,
                                 如果下個字元是 非該Token的格式,就確定以上的Token
                                 如果下個字元是 該Token的格式,就讀進input字元,之後先偷看下個字元,再做上一步
                                 

*/

// *******************************************************************************************************
// Token的格式
class Token
{
public:
  char mToken_Char ;
  int mToken_LINE ; // 記錄Token在行數
  int mToken_COLUMN ; // 記錄Token在個數
}; // Token

class Form_Of_Token // 每個Token的"格式"
{
  public:
  vector<Token> mToken ; // 記錄Token本身
  string mToken_TYPE ; // 記錄Token的形態
  // string mToken_VALUE ; // 記錄Token的值 


  string Token_ID()
  {
    string buffer = "" ;
    for ( int i = 0; i < mToken.size() ; i++ )
    {
      if ( buffer.size() == 0 )
      {
        buffer = mToken[i].mToken_Char ;
      } // if
      else
      {
        buffer = buffer + mToken[i].mToken_Char ;
      } // else 
    } // for

    return buffer ;
  } // Token_ID()
} ; // Form_Of_Token
// ********************************************************************************************************

// ********************************************************************************************************
// 指令樹中的Node
class Node
{
  public:
  string mstruct_type; // 記下作為StructNode是哪一種 INFORMATION/STRUCT/QUOTE/NIL
  // 如果是Root, Root是屬於STRUCT

  // 只有STRUCT有Left,Right
  Node* mleft ;
  Node* mright ;

  // 全部mstruct_type都有mparent
  Node* mparent;

  // 只有INFORMATION 有 minformation
  Form_Of_Token minformation;

  // 只有NIL 有 mneed_to_print_nil 決定要不要印
  bool mneed_to_print_nil ;
}; // Node

// ********************************************************************************************************

// ********************************************************************************************************
// Scanner 

class Scanner
{
  public:
  int mCurrent_Line, mCurrent_Column ; // 記錄現時行數和個數
  int mScanner_Token_Line, mScanner_Token_Column ;
  string mTemp_String ; // 暫存Input用
  char mTemp_Char ; // 暫存Input字元用
  vector<Form_Of_Token> mBuffer_Token ; // 用來暫存Token (還不確定是不是指令的)

  char Get_next_char() // 讀入下個char(讀走space) 
  {
    // 負責keep track of所get到的char的line number and column number
    char next_char ;
    string error_message ;
    int temp_line_counter = 0 ;
    int temp_column_counter = 0;
    bool is_only_waste_in_line = false;
    bool have_new_line = false;
    if ( cin.eof() )
    {
      error_message = "ERROR (no more input) : END-OF-FILE encountered" ;
      throw error_message ;
    } // if 

    while ( cin.peek() == ' ' || cin.peek() == '\t' || cin.peek() == '\n' ) 
    {
      // 讀走空白和換行
      if ( cin.peek() == '\n' )
      {
        have_new_line = true ;
        mCurrent_Line += 1 ;
        mCurrent_Column = 0 ;
        mScanner_Token_Column = 0 ;
        next_char = cin.get() ;
      } // if
      else if ( cin.eof() )
      {
        error_message = "ERROR (no more input) : END-OF-FILE encountered";
        throw error_message;
      } // else if 
      else
      {
        if ( have_new_line == true ) // 是新的一行
        {
          is_only_waste_in_line = true ; // 在一行中只有white space
        } // if
        else
        {
          is_only_waste_in_line = false ;
        } // else 
        
        next_char = cin.get();
        mScanner_Token_Column += 1 ;
        mCurrent_Column += 1 ;
      } // else 
    } // while

    if ( cin.eof() ) // ERROR (no more input) TYPE 4
    {
      error_message = "ERROR (no more input) : END-OF-FILE encountered";
      throw error_message;
    } // if
    else 
    {
      next_char = cin.get(); // 讀第1個有效字元
      return next_char;
    } // else 
  } // Get_next_char()

  char Get_next_non_white_char() // 讀入下個first char of Token 
  {
    mTemp_Char = Get_next_char();
    return mTemp_Char;
  } // Get_next_non_white_char()

  Form_Of_Token Set_Form_Token( string input_token, string input_token_type )
  {
    Form_Of_Token temp_token_from;
    Token temp_token;
    temp_token_from.mToken_TYPE = input_token_type ;
    for ( int i = 0; i < input_token.size() ; i++ )
    {
      temp_token.mToken_Char = input_token[i];
      temp_token.mToken_COLUMN = ( ( mCurrent_Column - input_token.size() ) + ( i + 1 ) );
      temp_token.mToken_LINE = mCurrent_Line;
      temp_token_from.mToken.push_back( temp_token );
    } // for

    return temp_token_from;
  } // Set_Form_Token()

  Form_Of_Token Def_Token( char first_char ) // 根據first_char決定它是甚麼Token
  {
    // 原則是收到First_char後找是甚麼Token,找到就會建好Form立刻回傳
    char buffer ; // 用於記錄下一個char (一般是peek出來的結果)
    char waste;
    string buildup_Token ; // 用於合成現時處理的Token 
    string buildup_Token_for_symbol = "" ;
    string buildup_Escape_Token; // 用於合成現時處理的Escape_Token 
    string error_message_type3; // "ERROR (no closing quote) : END-OF-LINE encountered at Line X Column"
    string error_message_type4; // "ERROR (no more input) : END-OF-FILE encountered"
    Form_Of_Token temp_Buildup_Form;
    mScanner_Token_Column = 0 ;
    bool have_symbol = false; // 用於決定是否是SYMBOL
    bool have_float = false; // 用於決定是否是FLOAT
    bool have_Command = false; // 用於決定是否有COMMAND
    bool enough_dot = false;
    buildup_Token.clear();
    buildup_Escape_Token.clear();
    //  現時可能是LEFT_PAREN, LEFT_PAREN_RIGHT_PAREN
    if ( first_char == '(' ) //  現時可能是LEFT_PAREN, LEFT_PAREN_RIGHT_PAREN
    {
      if ( cin.eof() )
      {
        error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
        throw error_message_type4;
      } // if

      // buffer = cin.peek(); // 偷看下一個字元 (可能有whitesapce)
      buffer = Get_next_non_white_char();
      if ( buffer == ')' ) // 是LEFT_PAREN_RIGHT_PAREN
      {
        mCurrent_Column += 1 ;
        mCurrent_Column = mCurrent_Column - mScanner_Token_Column ; 
        buildup_Token = first_char;
        mCurrent_Column += 1 ;
        buildup_Token = buildup_Token + buffer;
        // 建構Token
        return Set_Form_Token( buildup_Token, "LEFT_PAREN_RIGHT_PAREN" );
      } // if
      else // 是LEFT_PAREN
      {
        mCurrent_Column += 1 ;
        mCurrent_Column = mCurrent_Column - mScanner_Token_Column ; 
        cin.putback( buffer );
        buildup_Token = first_char;
        // 建構Token
        return Set_Form_Token( buildup_Token, "LEFT_PAREN" );
      } // else 
    } // if
    // 是RIGHT_PAREN
    else if ( first_char == ')' ) // 是RIGHT_PAREN
    {
      mCurrent_Column += 1 ;
      buildup_Token = first_char ;
      // 建構Token
      return Set_Form_Token( buildup_Token, "RIGHT_PAREN" );
    } // else if 
    // 是QUOTE
    else if ( first_char == '\'' ) // 是QUOTE
    {
      mCurrent_Column += 1 ;
      buildup_Token = first_char;
      // 建構Token
      return Set_Form_Token( buildup_Token, "QUOTE" );
    } // else if 
    // 可能是STRING 或 ERROR (no closing quote) Type 3
    else if ( first_char == '"' ) // 可能是STRING 或 ERROR
    {
      mCurrent_Column += 1 ;
      buildup_Token = first_char;
      while ( cin.peek() != '"' && cin.peek() != '\n' && !cin.eof() ) // 這while是看enter的換行和"
      {
        if ( cin.eof() )
        {
          error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
          throw error_message_type4;
        } // if

        if ( cin.peek() == '\\' )
        {
          buffer = cin.get();
          mCurrent_Column++;
          buildup_Token = buildup_Token + buffer;
          buffer = cin.get();
          mCurrent_Column++;
          buildup_Token = buildup_Token + buffer;
        } // if 
        else // 非\時
        {
          buffer = cin.get();
          mCurrent_Column++;
          buildup_Token = buildup_Token + buffer;
        } // else 

      } // while

      if ( cin.eof() )
      {
        error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
        throw error_message_type4;
      } // if

      if ( cin.peek() == '"' ) // 是完整""的STRING
      {
        buffer = cin.get();
        mCurrent_Column++;
        buildup_Token = buildup_Token + buffer;
        // 建構Token
        return Set_Form_Token( buildup_Token, "STRING" );
      } // if
      else // 理論上一定要係'\n'
      {
        buildup_Token.clear();
        // throw 錯誤
        mCurrent_Column++;
        string num_Colummn;
        string num_Line;
        stringstream ss;
        stringstream st;
        ss << mCurrent_Column ;
        ss >> num_Colummn;
        st << mCurrent_Line;
        st >> num_Line;
        error_message_type3 = "ERROR (no closing quote) : END-OF-LINE encountered at Line " + num_Line 
        + " Column " + num_Colummn ;
        throw error_message_type3;
      } // else 
    } // else if 
    // 先處理沒有+-的INT/Float和SYMBOL(數字開頭) 有處理沒有+-數字中出現;
    else if ( first_char >= '0' && first_char <= '9' ) // 先處理沒有+-的INT/Float和SYMBOL(數字開頭) 有處理沒有+-數字中出現;
    {
      mCurrent_Column += 1 ;
      buildup_Token = first_char;
      while ( cin.peek() != ';' && cin.peek() != ' ' && cin.peek() != '\t' && cin.peek() != '\n' 
              && cin.peek() != '\"' && cin.peek() != '\'' && cin.peek() != '(' 
              && cin.peek() != ')' && !cin.eof() ) 
      {
        if ( cin.eof() )
        {
          error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
          throw error_message_type4;
        } // if

        if ( cin.peek() < '0' || cin.peek() > '9' )
        {
          if ( cin.eof() )
          {
            error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
            throw error_message_type4;
          } // if

          if ( cin.peek() == '.' )
          {
            if ( have_float == true )
            {
              enough_dot = true;
            } // if
            else
            {
              have_float = true;
            } // else 

          } // if
          else if ( cin.peek() == ';' )
          {
            have_Command = true;
          } // else if 
          else
          {
            have_symbol = true;
          } // else 
        } // if
        

        if ( have_Command == true )
        {
          // Clear_line();
          CommanReader() ;
          have_Command = false;
        } // if
        else
        {
          buffer = cin.get();
          mCurrent_Column++;
          buildup_Token = buildup_Token + buffer;
        } // else 

      } // while
      
      if ( cin.peek() == ';' )
      {
        CommanReader() ;
      } // if 

      if ( have_float == true && have_symbol == true && enough_dot == true ) // 是SYMBOL e.g. 01.a.g 
      {
        have_float = false;
        have_symbol = false;
        return Set_Form_Token( buildup_Token, "SYMBOL" );
      } // if
      else if ( have_float == true && have_symbol == false && enough_dot == true ) // 是FLOAT 05.1.1
      {
        return Set_Form_Token( buildup_Token, "SYMBOL" );
      } // else if
      else if ( have_float == true && have_symbol == true ) // 是SYMBOL e.g. 2g.a
      {
        have_float = false;
        have_symbol = false;
        return Set_Form_Token( buildup_Token, "SYMBOL" );
      } // else if
      else if ( have_float == true && have_symbol == false ) // 是FLOAT 05.1
      {
        return Set_Form_Token( buildup_Token, "FLOAT" );
      } // else if
      else if ( have_float == false && have_symbol == true ) // 是SYMBOL e.g. 123$231ad32
      {
        have_float = false;
        have_symbol = false;
        return Set_Form_Token( buildup_Token, "SYMBOL" );
      } // else if 
      else if ( have_float == false && have_symbol == false ) // 是INT
      {
        have_float = false;
        have_symbol = false;
        return Set_Form_Token( buildup_Token, "INT" );
      } // else if 
      else
      {
        // 不會出現
      } // else 
    } // else if
    // 先處理有+- 可能有INT/FLOAT/SYMBOL
    else if ( first_char == '+' || first_char == '-' ) // 先處理有+- 可能有INT/FLOAT/SYMBOL 
    {
      mCurrent_Column += 1 ;
      buildup_Token = first_char;
      if ( cin.peek() >= '0' && cin.peek() <= '9' ) // 是+-的INT/Float/SYMBOL(數字開頭)
      {
        if ( cin.eof() )
        {
          error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
          throw error_message_type4;
        } // if

        while ( cin.peek() != ';' && cin.peek() != ' ' && cin.peek() != '\t' && cin.peek() != '\n' 
                && cin.peek() != '\"' && cin.peek() != '\'' && cin.peek() != '(' 
                && cin.peek() != ')' && !cin.eof() ) 
        {
          if ( cin.eof() )
          {
            error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
            throw error_message_type4;
          } // if

          if ( cin.peek() < '0' || cin.peek() > '9' )
          {
            if ( cin.eof() )
            {
              error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
              throw error_message_type4;
            } // if

            if ( cin.peek() == '.' )
            {
              if ( have_float == true )
              {
                enough_dot = true;
              } // if
              else
              {
                have_float = true;
              } // else 
                
            } // if
            else if ( cin.peek() == ';' )
            {
              CommanReader() ;
              have_Command = true;
            } // else if 
            else
            {
              have_symbol = true;
            } // else 
          } // if

          if ( have_Command == true )
          {
            Clear_line();  // 把Command和同行未讀的讀走 不會讀走換行
            have_Command = false;
          } // if
          else 
          {
            buffer = cin.get();
            mCurrent_Column++;
            buildup_Token = buildup_Token + buffer;
          } // else 

        } // while

        if ( cin.peek() == ';' )
        {
          CommanReader() ;
        } // if 

        if ( have_float == true && have_symbol == true && enough_dot == true ) 
        // 是SYMBOL e.g. +01.asds.a%dasd234
        {
          have_float = false;
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "SYMBOL" );
        } // if
        else if ( have_float == true && have_symbol == false && enough_dot == true ) // 是SYMBOL e.g. +0.1.3
        {
          have_float = false;
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "SYMBOL" );
        } // else if
        else if ( have_float == true && have_symbol == false && enough_dot == false ) // 是FLOAT +0.5
        {
          have_float = false;
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "FLOAT" );
        } // else if
        else if ( have_float == true && have_symbol == true ) // 是SYMBOL e.g. +01.asds.a%dasd234
        {
          have_float = false;
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "SYMBOL" );
        } // else if
        else if ( have_float == false && have_symbol == true ) // 是SYMBOL e.g. -123$231ad32
        {
          have_float = false;
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "SYMBOL" );
        } // else if 
        else if ( have_float == false && have_symbol == false ) // 是INT +5
        {
          have_float = false;
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "INT" );
        } // else if 
        else
        {
          // 不會出現
        } // else
      } // if
      else if ( cin.peek() == '.' ) // 處理+-開頭的. 有可能FLOAT/SYMBOL
      {
        mCurrent_Column += 1 ;
        buffer = cin.get();
        mCurrent_Column++;
        buildup_Token = buildup_Token + buffer; // +. -.
        have_float = true;
        if ( cin.peek() >= '0' && cin.peek() <= '9' ) // +.4 -.4dfsd
        {
          while ( cin.peek() != ';' && cin.peek() != ' ' && cin.peek() != '\t' && cin.peek() != '\n' 
                  && cin.peek() != '\"' && cin.peek() != '\'' && cin.peek() != '(' 
                  && cin.peek() != ')' && !cin.eof() ) 
          {
            if ( cin.peek() < '0' || cin.peek() > '9' )
            {
              if ( cin.eof() )
              {
                error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
                throw error_message_type4;
              } // if

              if ( cin.peek() == '.' )
              {
                if ( have_float == true )
                {
                  enough_dot = true;
                } // if
                else
                {
                  have_float = true;
                } // else 

              } // if
              else if ( cin.peek() == ';' )
              {
                CommanReader() ;
                have_Command = true;
              } // else if 
              else
              {
                have_symbol = true;
              } // else 
            } // if

            if ( have_Command == true )
            {
              Clear_line();  // 把Command和同行未讀的讀走 不會讀走換行
              have_Command = false;
            } // if
            else 
            {
              buffer = cin.get();
              mCurrent_Column++;
              buildup_Token = buildup_Token + buffer;
            } // else 

          } // while

          if ( cin.peek() == ';' )
          {
            // cout << "is command1\n" ;
            CommanReader() ;
          } // if 

          if ( have_symbol == true && enough_dot == true ) // 是SYMBOL e.g. +.4d +.1.1d
          {
            have_symbol = false;
            return Set_Form_Token( buildup_Token, "SYMBOL" );
          } // if
          else if ( have_symbol == true && enough_dot == false ) // 是SYMBOL e.g. +.4d 
          {
            have_symbol = false;
            return Set_Form_Token( buildup_Token, "SYMBOL" );
          } // else if
          else if ( have_symbol == false && enough_dot == true ) // 是FLOAT -.5.5
          {
            have_symbol = false;
            return Set_Form_Token( buildup_Token, "SYMBOL" );
          } // else if 
          else if ( have_symbol == false && enough_dot == false ) // 是FLOAT -.6
          {
            have_symbol = false;
            return Set_Form_Token( buildup_Token, "FLOAT" );
          } // else if 
        } // if
        else // +.a -.#d
        {
          have_float = true;
          while ( cin.peek() != ';' && cin.peek() != ' ' && cin.peek() != '\t' && cin.peek() != '\n' 
                  && cin.peek() != '\"' && cin.peek() != '\'' && cin.peek() != '(' 
                  && cin.peek() != ')' && !cin.eof() ) 
          {
            if ( cin.peek() == ';' )
            {
              CommanReader() ;  // 把Command和同行未讀的讀走 不會讀走換行
            } // if

            buffer = cin.get();
            mCurrent_Column++;
            buildup_Token = buildup_Token + buffer;
          } // while
          
          if ( cin.peek() == ';' )
          {
            // cout << "is command1\n" ;
            CommanReader() ;
          } // if

          return Set_Form_Token( buildup_Token, "SYMBOL" );
        } // else 
      } // else if
      else // 處理+- 後的符號 是SYMBOL
      {
        // e.g. +D32F -%
        while ( cin.peek() != ';' && cin.peek() != ' ' && cin.peek() != '\t' && cin.peek() != '\n' 
                && cin.peek() != '\"' && cin.peek() != '\'' && cin.peek() != '(' 
                && cin.peek() != ')' && !cin.eof() ) 
        {
          if ( cin.peek() == ';' )
          {
            CommanReader();  // 把Command和同行未讀的讀走 不會讀走換行
          } // if

          buffer = cin.get();
          mCurrent_Column++;
          buildup_Token = buildup_Token + buffer;
        } // while

        if ( cin.peek() == ';' )
        {
          // cout << "is command1\n" ;
          CommanReader() ;
        } // if

        return Set_Form_Token( buildup_Token, "SYMBOL" );
      } // else 
    } // else if
      // 處理開頭Dot 可能FLOAT/SYMBOL/DOT
    else if ( first_char == '.' )
    {
      mCurrent_Column += 1 ;
      buildup_Token = first_char;
      have_float = true;
      if ( cin.peek() >= '0' && cin.peek() <= '9' ) // 是FLOAT
      {
        buffer = cin.get();
        mCurrent_Column++;
        buildup_Token = buildup_Token + buffer;
        while ( cin.peek() != ';' && cin.peek() != ' ' && cin.peek() != '\t' 
                && cin.peek() != '\n' && cin.peek() != '\"'
                && cin.peek() != '\'' && cin.peek() != '(' && cin.peek() != ')' && !cin.eof() )
        {
          if ( cin.eof() )
          {
            error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
            throw error_message_type4;
          } // if

          if ( cin.peek() < '0' || cin.peek() > '9' )
          {
            if ( cin.eof() )
            {
              error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
              throw error_message_type4;
            } // if

            if ( cin.peek() == '.' )
            {
              enough_dot = true;
            } // if
            else if ( cin.peek() == ';' )
            {
              CommanReader() ;
              have_Command = true;
            } // else if 
            else
            {
              have_symbol = true;
            } // else 
          } // if 

          if ( have_Command == true )
          {
            Clear_line();  // 把Command和同行未讀的讀走 不會讀走換行
            have_Command = false;
          } // if
          else // 沒有Command的情況
          {
            buffer = cin.get();
            mCurrent_Column++;
            buildup_Token = buildup_Token + buffer;
          } // else 
        } // while

        if ( cin.peek() == ';' )
        {
          // cout << "is command1\n" ;
          CommanReader() ;
        } // if

        if ( have_symbol == false && enough_dot == true ) // 是FLOAT .6.4
        {
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "SYMBOL" );
        } // if 
        else if ( have_symbol == false && enough_dot == false ) // 是FLOAT .6
        {
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "FLOAT" );
        } // else if 
        else // 是SYMBOL e.g. .4s
        {
          have_symbol = false;
          return Set_Form_Token( buildup_Token, "SYMBOL" );
        } // else 
      } // if
      else if ( cin.peek() == ';' || cin.peek() == ' ' || cin.peek() == '\t' 
                || cin.peek() == '\n' || cin.peek() == '\"'
                || cin.peek() == '\'' || cin.peek() == '(' || cin.peek() == ')' ) // 是DOT
      {
        if ( cin.peek() == ';' )
        {
          CommanReader() ;
        } // if

        return Set_Form_Token( buildup_Token, "DOT" );
      } // else if 
      else // 是SYMBOL e.g. +d
      {
        buildup_Token = first_char;
        while ( cin.peek() != ';' && cin.peek() != ' ' && cin.peek() != '\t' 
                && cin.peek() != '\n' && cin.peek() != '\"'
                && cin.peek() != '\'' && cin.peek() != '(' && cin.peek() != ')' && !cin.eof() ) 
        {
          if ( cin.peek() == ';' )
          {
            CommanReader() ;
            have_Command = true;
          } // if 

          if ( have_Command == true )
          {
            Clear_line();  // 把Command和同行未讀的讀走 不會讀走換行
            have_Command = false;
          } // if
          else // 沒有Command的情況
          {
            buffer = cin.get();
            mCurrent_Column++;
            buildup_Token = buildup_Token + buffer;
          } // else 
        } // while

        if ( cin.peek() == ';' )
        {
          CommanReader() ;
        } // if

        return Set_Form_Token( buildup_Token, "SYMBOL" );
      } // else 
    } // else if 
    // 處理Command
    else if ( first_char == ';' )
    {
      CommanReader();
      return Def_Token( Get_next_non_white_char() );
    } // else if 
    // 處理SYMBOL
    else // 處理SYMBOL
    {
      mCurrent_Column += 1 ;
      buildup_Token = first_char;
      while ( cin.peek() != ' ' && cin.peek() != '\t' && cin.peek() != '\n' 
              && cin.peek() != '(' && cin.peek() != ')' 
              && cin.peek() != '\"' && cin.peek() != '\'' && !cin.eof() ) 
      {
        if ( cin.eof() )
        {
          error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
          throw error_message_type4;
        } // if

        if ( cin.peek() == ';' )
        {
          have_Command = true;
        } // if 

        if ( have_Command == true )
        {
          Clear_line();  // 把Command和同行未讀的讀走 不會讀走換行
          have_Command = false;
        } // if
        else // 沒有Command的情況
        {
          buffer = cin.get();
          mCurrent_Column++;
          buildup_Token = buildup_Token + buffer;
        } // else 
      } // while

      if ( buildup_Token == "#f" ) // 是NIL
      {
        return Set_Form_Token( buildup_Token, "NIL" );
      } // if
      else if ( buildup_Token == "nil" )
      {
        return Set_Form_Token( buildup_Token, "NIL" );
      } // else if 
      else if ( buildup_Token == "#t" ) // 是T
      {
        return Set_Form_Token( buildup_Token, "T" );
      } // else if
      else if ( buildup_Token == "t" ) // 是T
      {
        return Set_Form_Token( buildup_Token, "T" );
      } // else if
      else
      {
        return Set_Form_Token( buildup_Token, "SYMBOL" );
      } // else 
    } // else 
      
    return temp_Buildup_Form;
  } // Def_Token()

  Form_Of_Token Get_Token() // 回傳Token () 
  {
    return mBuffer_Token[mBuffer_Token.size()-1];
  } // Get_Token()

  Form_Of_Token Peek_Next_Token() // 偷看下個Token *主要Function*
  {
    char first_char = '\0'; // 作為記錄第1個char
    char waste; 
    Form_Of_Token readin_Token; // 作為記錄Def出來的Token
    bool have_char = false ;
    // 要令讀入的第一個字元為非空白字元
    first_char = Get_next_non_white_char();
    readin_Token = Def_Token( first_char );
    // 先Peek完後,先放在mBuffer_Token_Set中
    mBuffer_Token.push_back( readin_Token );
    return readin_Token;
  } // Peek_Next_Token()

  void Clear_line() // 把整行讀走
  {
    char waste;
    bool exit = false;
    string error_message_type4;
    // waste = cin.peek() ;
    while ( exit != true )
    {
      if ( !cin.eof() )
      {
        waste = cin.get();
      } // if 
      else
      {
        exit = true;
      } // else 

      if ( exit == false && waste == '\n' )
      {
        exit = true;
      } // if
    } // while  

    // cin >> waste;
    mCurrent_Line = 1;
    mCurrent_Column = 0;
  } // Clear_line()

  void CommanReader() // 只會被Command後使用,寫得有點亂
  {
    char temp_input_char;
    string error_message_type4;
    if ( cin.eof() )
    {
      error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
      throw error_message_type4;
    } // if

    while ( cin.peek() != '\n' )
    {
      if ( cin.eof() )
      {
        error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
        throw error_message_type4;
      } // if


      temp_input_char = cin.get();
    } // while      

    if ( cin.eof() )
    {
      // cout << "hello2\n";
      error_message_type4 = "ERROR (no more input) : END-OF-FILE encountered";
      throw error_message_type4;
    } // if
    else
    {
      temp_input_char = cin.get();
      mCurrent_Line++ ;
      mCurrent_Column = 0;
    } // else 
  } // CommanReader()
}; // Scanner

// ********************************************************************************************************

// ********************************************************************************************************
// Parser

class Parser
{
  public:
  Scanner mscanner ; 
  vector<Form_Of_Token> mlist_of_exp ; // 記錄1串指令
  bool mdot_Right_Parent ; 
  bool mRight_Parent_safety_buffer ; 
  void ReadSExp( Form_Of_Token &input_token ) // 讓Main()呼叫
  {
    Form_Of_Token temp_token;
    temp_token = mscanner.Peek_Next_Token();
    CheckSchemeGrammer( temp_token );
    input_token = temp_token;
  } // ReadSExp()

  void CheckSchemeGrammer( Form_Of_Token temp_token ) // 檢查文法
  {
    string error_message; // 是記錄錯誤信息 (用來throw)
    stringstream sl;
    stringstream sc;
    string num_Line;
    string num_Column;

    if ( Is_SExp( temp_token ) == true )
    {
      if ( mscanner.mBuffer_Token.size() == 3 )
      {
        if ( mscanner.mBuffer_Token[0].Token_ID() == "(" && mscanner.mBuffer_Token[1].Token_ID() == "exit" 
             && mscanner.mBuffer_Token[2].Token_ID() == ")" )
        {
          error_message = "(exit)";
          throw error_message ;
        } // if  
      } // if

      mlist_of_exp = mscanner.mBuffer_Token; // 把Scanner中之前未確定的指令給list_of_exp 
      mscanner.mCurrent_Line = 1;
      mscanner.mCurrent_Column = 0;
      mscanner.mBuffer_Token.clear();
    } // if
    else
    {
      sl << mscanner.mCurrent_Line;
      sl >> num_Line;
      sc << mscanner.mCurrent_Column;
      sc >> num_Column;
      error_message = "ERROR (unexpected token) : atom or '(' expected when token at Line " + num_Line 
      + " Column " + num_Column + " is >>" + temp_token.Token_ID() + "<<";
      throw error_message;
    } // else 

  } // CheckSchemeGrammer()

  // ******************************************************
  // 屬於Parser 1:檢查文法

  bool Is_ATOM( Form_Of_Token temp_token ) // 檢查是否ATOM
  {
    /*
    <ATOM> ::= SYMBOL | INT | FLOAT | STRING | NIL | T | LEFT-PAREN RIGHT-PAREN
    */
    if ( temp_token.mToken_TYPE == "SYMBOL" || temp_token.mToken_TYPE == "INT" 
         || temp_token.mToken_TYPE == "FLOAT"
         || temp_token.mToken_TYPE == "STRING" 
         || temp_token.mToken_TYPE == "NIL" || temp_token.mToken_TYPE == "T"
         || temp_token.mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN" )
    {
      // cout << temp_token.Token_ID() <<" is ATOM\n";
      return true ;
    } // if
    else
    {
      return false ;
    } // else 
  } // Is_ATOM()

  bool Is_SExp( Form_Of_Token temp_token )
  {
    /*
    <S-EXP> ::= <ATOM> | 
                LEFT-PAREN <S-EXP> {<S-EXP>} [ DOT <S-EXP> ] RIGHT-PAREN |
                QUOTE <S-EXP>
    */
    string error_message ; // 是記錄錯誤信息 (用來throw)
    stringstream sl ;
    stringstream sc ;
    string num_Line ;
    string num_Column ;
    // cout << "Start SExp\n";
    if ( Is_ATOM( temp_token ) == true )
    {
      // cout << "End ATOM\n";
      // cout << "End SExp\n";
      return true ;
    } // if
    else if ( temp_token.mToken_TYPE == "LEFT_PAREN" )
    {
      // cout << temp_token.Token_ID() << " is LEFT_PAREN\n";
      temp_token = mscanner.Peek_Next_Token();
      if ( Is_SExp( temp_token ) == true )
      {
        temp_token = mscanner.Peek_Next_Token();
        if ( Is_SExp( temp_token ) == true )
        {
          temp_token = mscanner.Peek_Next_Token();
          while ( Is_SExp( temp_token ) )
          {
            temp_token = mscanner.Peek_Next_Token() ;
          } // while
        } // if

        if ( temp_token.mToken_TYPE == "DOT" )
        {
          // cout << temp_token.Token_ID() << " is DOT\n";
          temp_token = mscanner.Peek_Next_Token();
          if ( Is_SExp( temp_token ) == true )
          {
            temp_token = mscanner.Peek_Next_Token();
          } // if
          else
          {
            // ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<< 
            // Type 1
            sl << mscanner.mCurrent_Line;
            sl >> num_Line;
            sc << mscanner.mCurrent_Column;
            sc >> num_Column;
            error_message = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
            + num_Line + " Column " + num_Column + " is >>" + temp_token.Token_ID() + "<<" ;
            throw error_message ;
          } // else 
        } // if

        if ( temp_token.mToken_TYPE == "RIGHT_PAREN" )
        {
          // cout << temp_token.Token_ID() << " is RIGHT_PAREN\n";
          // cout << "End SExp\n";
          return true;
        } // if
        else
        {
          // ERROR (unexpected token) : ')' expected when token at Line X Column Y is >>...<< 
          // Type 2
          sl << mscanner.mCurrent_Line;
          sl >> num_Line;
          sc << mscanner.mCurrent_Column;
          sc >> num_Column;
          error_message = "ERROR (unexpected token) : ')' expected when token at Line " 
          + num_Line + " Column " + num_Column + " is >>" + temp_token.Token_ID() + "<<" ;
          throw error_message;
        } // else
      } // if
      else
      {
        // ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<< 
        // Type 1
        sl << mscanner.mCurrent_Line;
        sl >> num_Line;
        sc << mscanner.mCurrent_Column;
        sc >> num_Column;
        error_message = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
        + num_Line + " Column " + num_Column + " is >>" + temp_token.Token_ID() + "<<" ;
        throw error_message ;
      } // else 
    } // else if 
    else if ( temp_token.mToken_TYPE == "QUOTE" )
    {
      // cout << temp_token.Token_ID() << " is QUOTE\n";
      temp_token = mscanner.Peek_Next_Token();
      if ( Is_SExp( temp_token ) == true )
      {
        // cout << "End SExp\n";
        return true;
      } // if
      else
      {
        // ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<< 
        // Type 1
        sl << mscanner.mCurrent_Line;
        sl >> num_Line;
        sc << mscanner.mCurrent_Column;
        sc >> num_Column;
        error_message = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
        + num_Line + " Column " + num_Column + " is >>" + temp_token.Token_ID() + "<<" ;
        throw error_message;
      } // else 
    } // else if 
    else
    {
      return false;
    } // else 

  } // Is_SExp()

  // ******************************************************

  // ******************************************************
  // Parser 2: 建立文法樹
  string Get_String_Value( Form_Of_Token temp_token )
  {
    // 負責把temp_token的ID變成值
    stringstream stoInt ;
    stringstream stoFloat ;
    stringstream inttoS;
    stringstream floattoS;

    string num_Int ;
    string num_Float ;
    int num_of_int = 0 ;
    int size_of_token = temp_token.mToken.size() ;
    float num_of_float = 0.0 ;
    int i = 0 ;
    // 把00009變成9, +0009=>9 , -04=>-4 
    if ( temp_token.mToken_TYPE == "INT" )
    {
      stoInt << temp_token.Token_ID() ;
      stoInt >> num_of_int ;
      inttoS << num_of_int;
      inttoS >> num_Int;
      return num_Int;
    } // if
    // 把00009.5變成9.5, +000.9=>0.9 , -0.4=> -0.4 
    else 
    {
      stoFloat << temp_token.Token_ID() ;
      // cout << temp_token.Token_ID() << endl ;
      
      if ( temp_token.mToken[0].mToken_Char == '+' || temp_token.mToken[0].mToken_Char == '-'  )
      {
        if ( temp_token.mToken[0].mToken_Char == '-' )
        {
          num_Float = "-" ;
        } // if 
        
        i += 1 ;
      } // if 
      
      for ( int u = i ; u < size_of_token ; u++ )
      {
        if ( u != 0 )
        {
          num_Float = num_Float + temp_token.mToken[u].mToken_Char ;
        } // if 
        else 
        {
          num_Float = temp_token.mToken[u].mToken_Char  ;
        } // else 

        if ( temp_token.mToken[u].mToken_Char == '.' )
        {
          if ( u+1 == size_of_token )
          {
            num_Float = num_Float + "000" ;
          } // if 
        } // if 
        
      } // for

      /*
      stoFloat >> num_of_float ;
      cout << num_of_float << endl ;
      floattoS << num_of_float ;
      floattoS >> num_Float ;
      */

      
      return num_Float ;
    } // else if 

  } // Get_String_Value()

  void Print_String( Form_Of_Token temp_token )
  {
    int size_of_string = temp_token.mToken.size() ;
    for ( int i = 0 ; i < size_of_string ; i++ )
    {
      char temp_char = temp_token.mToken[i].mToken_Char ;
      if ( temp_char == '\\' )
      {
        if ( temp_token.mToken[i+1].mToken_Char == 'n' )
        {
          // 是\n
          i += 1 ;
          cout << "\n" ;
        } // if 
        else if ( temp_token.mToken[i+1].mToken_Char == 't' )
        {
          // 是\t
          i += 1 ;
          cout << "\t" ;
        } // else if
        else if ( temp_token.mToken[i+1].mToken_Char == '"' )
        {
          // 是

          i += 1 ;
          cout << '"' ;
        } // else if 
        else if ( temp_token.mToken[i+1].mToken_Char == '\\' )
        {
          // 是
          
          cout << '\\' ;
          if ( temp_token.mToken[i+2].mToken_Char != '\\' )
          {
            cout << temp_token.mToken[i+2].mToken_Char ;
          } // if
          
          i += 2 ;
        } // else if 
        else 
        {
          cout << temp_char ;
        } // else 
      } // if 
      else 
      {
        cout << temp_char ;
      } // else 
    } // for

    cout << endl ;
  } // Print_String()

  Node* CreateStructNode()
  {
    // 建設STRUCTURE Node
    // 包括設mleft和mright為NULL, mstruct_type為STRUCT
    Node* newNode = new Node() ;
    newNode->mleft = NULL ;
    newNode->mright = NULL ;
    newNode->mstruct_type = "STRUCT";
    return newNode ;
  } // CreateStructNode()

  Node* CreateInformationNode( Form_Of_Token temp_token )
  {
    // 負責建構InformationNode 
    // 包括設mstruct_type為INFORMATION, minformation為temp_token
    Node* newNode = new Node() ;
    // cout << "YO :" << temp_token.mToken_TYPE << endl ;
    newNode->mstruct_type = "INFORMATION" ;
    newNode->minformation = temp_token ;
    return newNode;
  } // CreateInformationNode()

  Node* CreateNilNode()
  {
    // 負責建構NIL Node 
    // 包括設mstruct_type為NIL
    Node* newNode = new Node();
    newNode->mstruct_type = "NIL";
    return newNode;
  } // CreateNilNode()

  Node* CreateQuoteNode()
  {
    // 負責建構QUOTE Node 
    // 包括設mstruct_type為QUOTE
    Node* newNode = new Node();
    newNode->mstruct_type = "QUOTE";
    return newNode;
  } // CreateQuoteNode()

  Node* InsertStructNode( Node* current_node, string way )
  {
    // 用來加入StructNode
    // return 的pointer為StructNode
    Node* newNode = new Node() ;
    newNode = CreateStructNode() ;
    if ( way == "LEFT" )
    {
      current_node->mleft = newNode ;
    } // if 
    else if ( way == "RIGHT" )
    {
      current_node->mright = newNode ;
    } // else if 
    else 
    {
      // WHY
    } // else

    newNode->mparent = current_node; 
    return newNode ;
  } // InsertStructNode()

  Node* Buildup_Tree( Node* current_node, int &current_token_id )
  {
    // 負責建構文法樹 
    // 3大規則
    //   左邊空,放左邊
    //   右邊空 有Dot就放右邊
    //         沒有Dot就把current_node的mright接上一個新的Struct,再用該Struct作Buildup_Tree中的current_node
    // 如果current_token_id 是-1,就等於讀完
    
    Form_Of_Token temp_token ; // 為現時處理中的Token 
    Node* temp_node ; // 為暫存的Node
    Node* temp_node2 ; 
    Node* temp_node3 ;
    Node* temp_node4 ;
    Node* temp_node5 ;
    int count_for_paren ;
    // mdot_Right_Parent = false ; 
    // cout << "Now the token_id is " << current_token_id << endl ;
    // cout << mlist_of_exp.size() << endl ;

    // *******************************************************
    // Recursion的底
    if ( current_token_id == -1 ) // 當指令串讀完
    {
      // cout << "All of the tokenes have been readed\n" ;
      if ( current_node->mparent == NULL ) // 回傳到Root
      {
        return current_node ;
      } // if 
      else // 回傳現時Node的parent到上一層
      {
        return Buildup_Tree( current_node->mparent, current_token_id );
      } // else if 
    } // if 

    if ( current_token_id != 0 && current_node->mparent == NULL )
    {
      // 當出現當指令串還未讀完,但已經沒有多的位置
      return current_node ;
    } // if 
    // ********************************************************

    // ********************************************************
    // 取Token 
    if ( current_token_id >= mlist_of_exp.size()-1 )
    {
      if ( current_token_id == mlist_of_exp.size()-1 )
      {
        // 在取最後一個Token
        temp_token = mlist_of_exp[current_token_id] ;
        // cout << "I took " << mlist_of_exp[current_token_id].Token_ID() << endl << "It is the last token\n"
      } // if 

      current_token_id = -1 ;
    } // if 
    else 
    {
      // 正常取Token
      temp_token = mlist_of_exp[current_token_id] ;
      // cout << "I took " << mlist_of_exp[current_token_id].Token_ID() << endl ;
    } // else 

    // ********************************************************
    // 決定是要用甚麼規則
    if ( current_node->mleft == NULL ) // 左邊是NULL 規則1
    {
      // cout << "Here is left\n" ;
      if ( temp_token.mToken_TYPE == "LEFT_PAREN" ) 
      {
        if ( mdot_Right_Parent == true )
        {
          mRight_Parent_safety_buffer = true ;
          mdot_Right_Parent = false ;
        } // if 

        temp_node = InsertStructNode( current_node, "LEFT" ) ;
        current_token_id += 1 ;
        return Buildup_Tree( temp_node, current_token_id ) ;
      } // if 
      else if ( temp_token.mToken_TYPE == "QUOTE" )
      {
        temp_node = InsertStructNode( current_node, "LEFT" ) ;
        temp_node2 = CreateQuoteNode() ;
        temp_node2->mparent = temp_node ;
        temp_node->mleft = temp_node2 ;

        current_token_id += 1 ;
        if ( current_token_id <= mlist_of_exp.size() -1 )
        {
          if ( mlist_of_exp[current_token_id].mToken_TYPE != "QUOTE" )
          {
            if ( mlist_of_exp[current_token_id].mToken_TYPE == "INT" 
                 || mlist_of_exp[current_token_id].mToken_TYPE == "FLOAT" 
                 || mlist_of_exp[current_token_id].mToken_TYPE == "STRING" 
                 || mlist_of_exp[current_token_id].mToken_TYPE == "SYMBOL"
                 || mlist_of_exp[current_token_id].mToken_TYPE == "T"
                 || mlist_of_exp[current_token_id].mToken_TYPE == "NIL"
                 || mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN"   )
            {
              temp_token = mlist_of_exp[current_token_id] ;
              temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
              if ( mlist_of_exp[current_token_id].mToken_TYPE == "NIL" 
                   || mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN" )
              {
                temp_node4 = CreateNilNode() ; 
              } // if 
              else 
              {
                temp_node4 = CreateInformationNode( temp_token ) ;
              } // else 

              temp_node4->mparent = temp_node3 ;
              temp_node3->mleft = temp_node4 ; 
              temp_node5 = CreateNilNode() ; 
              temp_node5->mparent = temp_node3 ;
              temp_node3->mright = temp_node5 ;
              temp_node3->mparent = temp_node ; 
              // cout << "yo2 " << temp_node3->mleft->minformation.Token_ID() << endl ; 
   
              current_token_id += 1 ;
             
              return Buildup_Tree( temp_node, current_token_id ) ;
            } // if 
            else if ( mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN" )
            {
              // temp_token = mlist_of_exp[current_token_id] ;
              temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
              temp_node5 = CreateNilNode() ; 
              temp_node5->mparent = temp_node3 ;
              temp_node3->mright = temp_node5 ;
              temp_node3->mparent = temp_node ; 
              // current_token_id += 1 ;

              return Buildup_Tree( temp_node3, current_token_id ) ;
            } // else if 
            else 
            {
              return Buildup_Tree( temp_node, current_token_id ) ;
            } // else 
          } // if
          else 
          {
            return Buildup_Tree( temp_node, current_token_id ) ;
          } // else 
        } // if 

        return Buildup_Tree( temp_node, current_token_id ) ;
      } // else if 
      else if ( temp_token.mToken_TYPE == "NIL" || temp_token.mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN" )
      {
        temp_node = CreateNilNode() ;
        temp_node->mparent = current_node ;
        current_node->mleft = temp_node ;
        if ( current_token_id != -1 )
        {
          current_token_id += 1 ;
        } // if 

        return Buildup_Tree( current_node, current_token_id ) ;
      } // else if 
      else 
      {
        // 在加入INFORMATION Node
        temp_node = CreateInformationNode( temp_token ) ;
        // cout << "Yo2:" << temp_node->minformation.mToken_TYPE << endl;
        temp_node->mparent = current_node ;
        current_node->mleft = temp_node ;
        // cout << current_node->mleft->minformation.mToken_TYPE << endl ;
        if ( current_token_id != -1 )
        {
          current_token_id += 1 ;
        } // if 

        return Buildup_Tree( current_node, current_token_id ) ;
      } // else 
    } // if 
    else if ( current_node->mright == NULL ) // 右邊是NULL 
    {
      if ( temp_token.mToken_TYPE == "DOT" ) // 有DOT 規則3
      {
        int peek_token_id = current_token_id + 1 ;
        string peek_token_type = mlist_of_exp[peek_token_id].mToken_TYPE ;
        // 偷看下一個Token的Type是甚麼
        // 在DOT之後會出現的情況有 LEFT_PAREN/ATOM/QUOTE
        if ( peek_token_type == "LEFT_PAREN" ) 
        {
          // 例子 : ( a . ( b c ))
          // 正常取Token
          current_token_id += 1 ;
          temp_token = mlist_of_exp[current_token_id] ;
          mdot_Right_Parent = true ; 
          // cout << "I took " << mlist_of_exp[current_token_id].Token_ID() << endl ;

          temp_node = InsertStructNode( current_node, "RIGHT" ) ;
          current_token_id += 1 ;
          return Buildup_Tree( temp_node, current_token_id ) ;
        } // if 
        else if ( peek_token_type == "LEFT_PAREN_RIGHT_PAREN" || peek_token_type == "NIL" ) 
        {
          // 正常取Token
          current_token_id += 1 ;
          temp_token = mlist_of_exp[current_token_id] ;
          // cout << "I took " << mlist_of_exp[current_token_id].Token_ID() << endl ;

          temp_node = CreateNilNode() ;
          temp_node->mparent = current_node ;
          current_node->mright = temp_node ;
          peek_token_id = current_token_id + 1 ;
          if ( peek_token_id <= mlist_of_exp.size() -1 )
          {
            peek_token_type = mlist_of_exp[peek_token_id].mToken_TYPE ;
            if ( peek_token_type == "RIGHT_PAREN" )
            {
              if ( peek_token_id >= mlist_of_exp.size() -1 )
              {
                current_token_id = -1 ;
              } // if 
              else 
              {
                current_token_id = current_token_id + 2 ;
              } // else 
            } // if 
            else 
            {
              current_token_id += 1 ;
            } // else 
          } // if 
          else
          {
            current_token_id = -1 ;
          } // else 

          
          return Buildup_Tree( current_node, current_token_id ) ;
        } // else if 
        else if ( peek_token_type == "QUOTE" )
        {
          temp_node = InsertStructNode( current_node, "RIGHT" ) ;
          temp_node2 = CreateQuoteNode() ;
          temp_node2->mparent = temp_node ;
          temp_node->mleft = temp_node2 ;

          current_token_id += 1 ; // 第一個QUOTE
          current_token_id += 1 ; // 偷看下一個

          if ( current_token_id <= mlist_of_exp.size() -1 )
          {
            if ( mlist_of_exp[current_token_id].mToken_TYPE != "QUOTE" )
            {
              if ( mlist_of_exp[current_token_id].mToken_TYPE == "INT" 
                   || mlist_of_exp[current_token_id].mToken_TYPE == "FLOAT" 
                   || mlist_of_exp[current_token_id].mToken_TYPE == "STRING" 
                   || mlist_of_exp[current_token_id].mToken_TYPE == "SYMBOL"
                   || mlist_of_exp[current_token_id].mToken_TYPE == "T"
                   || mlist_of_exp[current_token_id].mToken_TYPE == "NIL"   )
              {
                temp_token = mlist_of_exp[current_token_id] ;
                temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
                temp_node3->mleft = CreateInformationNode( temp_token ) ;
                temp_node3->mright = CreateNilNode() ;
                temp_node3->mparent = temp_node ; 
                current_token_id += 1 ;
                return Buildup_Tree( temp_node, current_token_id ) ;
              } // if 
              else if ( mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN" )
              {
                // temp_token = mlist_of_exp[current_token_id] ;
                temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
                temp_node3->mright = CreateNilNode() ;
                temp_node3->mparent = temp_node ; 
                // current_token_id += 1 ;
                return Buildup_Tree( temp_node3, current_token_id ) ;
              } // else if 
              else 
              {
                return Buildup_Tree( temp_node, current_token_id ) ;
              } // else 
            } // if 
            else 
            {
              return Buildup_Tree( temp_node, current_token_id ) ;
            } // else 

            return Buildup_Tree( temp_node, current_token_id ) ;
          } // if 

          return Buildup_Tree( temp_node, current_token_id ) ;
        } // else if 
        else // 其他Token (應該只有INT/FLOAT/SYMBOL/STRING/T)
        {
          // 正常取Token
          current_token_id += 1 ; // 取其他Token
          temp_token = mlist_of_exp[current_token_id] ;
          // cout << "I took " << mlist_of_exp[current_token_id].Token_ID() << endl ;

          temp_node = CreateInformationNode( temp_token ) ;
          temp_node->mparent = current_node ;
          current_node->mright = temp_node ;

          peek_token_id = current_token_id + 1 ;
          if ( peek_token_id <= mlist_of_exp.size() -1 )
          {
            peek_token_type = mlist_of_exp[peek_token_id].mToken_TYPE ;
            if ( peek_token_type == "RIGHT_PAREN" )
            {
              if ( peek_token_id >= mlist_of_exp.size() -1 )
              {
                current_token_id = -1 ;
              } // if 
              else 
              {
                current_token_id = current_token_id + 2 ; 
              } // else 
            } // if 
          } // if 
          else
          {
            current_token_id = -1 ;
          } // else 

          return Buildup_Tree( current_node, current_token_id ) ;
        } // else 
      } // if 
      else if ( temp_token.mToken_TYPE == "RIGHT_PAREN" )
      {
        if ( current_token_id != -1 )
        {
          current_token_id += 1 ;
        } // if 
        
        if ( mRight_Parent_safety_buffer == true )
        {
          mdot_Right_Parent = true ;
          mRight_Parent_safety_buffer = false ;
        } // if 
        else if ( mdot_Right_Parent == true )
        {
          mdot_Right_Parent = false ;
          return Buildup_Tree( current_node, current_token_id ) ;
        } // else if 

        if ( current_node->mleft->mstruct_type == "INFORMATION" 
             || current_node->mleft->mstruct_type == "NIL" )
        {
          temp_node = CreateNilNode() ;
          temp_node->mparent = current_node ;
          current_node->mright = temp_node ;
          
          if ( current_token_id <= mlist_of_exp.size()-1 )
          {
            if ( mlist_of_exp[current_token_id].mToken_TYPE == "RIGHT_PAREN" )
            {
              return Buildup_Tree( current_node->mparent, current_token_id ) ;
            } // if 
          } // if 
          

          return Buildup_Tree( current_node, current_token_id ) ;
          // return Buildup_Tree( current_node->mparent, current_token_id ) ;
        } // if 
        else if ( current_node->mleft->mstruct_type == "STRUCT" )
        {
          temp_node2 = current_node->mleft ;
          if ( temp_node2->mleft->mstruct_type == "INFORMATION" 
               || temp_node2->mleft->mstruct_type == "NIL"
               || temp_node2->mleft->mstruct_type == "QUOTE" )
          {
            if ( temp_node2->mright->mstruct_type == "NIL" || temp_node2->mright->mstruct_type == "STRUCT" )
            {
              temp_node = CreateNilNode() ;
              temp_node->mparent = current_node ;
              current_node->mright = temp_node ;
              return Buildup_Tree( current_node, current_token_id ) ;
              // return Buildup_Tree( current_node->mparent, current_token_id ) ;
            } // if 
          } // if 
          /*
          if ( temp_node2->mleft->mstruct_type == "STRUCT" )
          {
            temp_node3 = temp_node2->mleft ;
            if ( temp_node3->mleft->mstruct_type == "QUOTE" )
            {
              temp_node = CreateNilNode() ;
              temp_node->mparent = current_node ;
              current_node->mright = temp_node ;
              return Buildup_Tree( current_node, current_token_id ) ;
            } // if 
          } // if 
          */
        } // else if 

        return Buildup_Tree( current_node, current_token_id ) ;
      } // else if 
      else if ( temp_token.mToken_TYPE == "QUOTE" )
      {
        // current_node的右邊是空, 遇到QUOTE, QUOTE之後的Token是QUOTE
        if ( current_token_id + 1 <= mlist_of_exp.size() -1 ) 
        {
          if ( mlist_of_exp[current_token_id-1].mToken_TYPE == "QUOTE" )
          {
            // 上一個Token是QUOTE
            current_token_id += 1 ;
            temp_node = InsertStructNode( current_node, "RIGHT" ) ;
            temp_node2 = CreateQuoteNode() ;
            temp_node2->mparent = temp_node ;
            temp_node->mleft = temp_node2 ;
            
            if ( current_token_id <= mlist_of_exp.size() -1 )
            {
              if ( mlist_of_exp[current_token_id].mToken_TYPE != "QUOTE" )
              {
                if ( mlist_of_exp[current_token_id].mToken_TYPE == "INT" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "FLOAT" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "STRING" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "SYMBOL"
                     || mlist_of_exp[current_token_id].mToken_TYPE == "T"
                     || mlist_of_exp[current_token_id].mToken_TYPE == "NIL" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN" )
                {
                  temp_token = mlist_of_exp[current_token_id] ;
                  temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
                  if ( mlist_of_exp[current_token_id].mToken_TYPE == "NIL" 
                       || mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN" )
                  {
                    temp_node4 = CreateNilNode() ; 
                  } // if 
                  else 
                  {
                    temp_node4 = CreateInformationNode( temp_token ) ;
                  } // else 
  
                  temp_node4->mparent = temp_node3 ;
                  temp_node3->mleft = temp_node4 ; 
                  temp_node5 = CreateNilNode() ; 
                  temp_node5->mparent = temp_node3 ;
                  temp_node3->mright = temp_node5 ;
                  temp_node3->mparent = temp_node ; 
    
                  current_token_id += 1 ;
                  return Buildup_Tree( temp_node, current_token_id ) ;
                } // if 
                else if ( mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN" )
                {
                  // temp_token = mlist_of_exp[current_token_id] ;
                  temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
                  temp_node5 = CreateNilNode() ; 
                  temp_node5->mparent = temp_node3 ;
                  temp_node3->mright = temp_node5 ;
                  temp_node3->mparent = temp_node ; 
                  // current_token_id += 1 ;

                  return Buildup_Tree( temp_node3, current_token_id ) ;
                } // else if 
                else 
                {
                  return Buildup_Tree( temp_node, current_token_id ) ;
                } // else 
              } // if 
              

              return Buildup_Tree( temp_node, current_token_id ) ;
            } // if 
            else 
            {
              // 不知道為何加
              return Buildup_Tree( temp_node, current_token_id ) ;
            } // else 
          } // if 
          else if ( mlist_of_exp[current_token_id+1].mToken_TYPE != "QUOTE" ) 
          {
            temp_node = InsertStructNode( current_node, "RIGHT" ) ;
            return Buildup_Tree( temp_node, current_token_id ) ;
          } // else if 
          else if ( mlist_of_exp[current_token_id+1].mToken_TYPE == "QUOTE" )
          {
            current_token_id += 1 ;
            temp_node = InsertStructNode( current_node, "RIGHT" ) ;
            temp_node2 = CreateQuoteNode() ;
            temp_node2->mparent = temp_node ;
            temp_node->mleft = temp_node2 ;
            current_token_id += 1 ;
            if ( current_token_id <= mlist_of_exp.size() -1 )
            {
              if ( mlist_of_exp[current_token_id].mToken_TYPE != "QUOTE" )
              {
                if ( mlist_of_exp[current_token_id].mToken_TYPE == "INT" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "FLOAT" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "STRING" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "SYMBOL"
                     || mlist_of_exp[current_token_id].mToken_TYPE == "T"
                     || mlist_of_exp[current_token_id].mToken_TYPE == "NIL" 
                     || mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN" )
                {
                  temp_token = mlist_of_exp[current_token_id] ;
                  temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
                  if ( mlist_of_exp[current_token_id].mToken_TYPE == "NIL" 
                       || mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN_RIGHT_PAREN" )
                  {
                    temp_node4 = CreateNilNode() ; 
                  } // if 
                  else 
                  {
                    temp_node4 = CreateInformationNode( temp_token ) ;
                  } // else 
  
                  temp_node4->mparent = temp_node3 ;
                  temp_node3->mleft = temp_node4 ; 
                  temp_node5 = CreateNilNode() ; 
                  temp_node5->mparent = temp_node3 ;
                  temp_node3->mright = temp_node5 ;
                  temp_node3->mparent = temp_node ; 
    
                  current_token_id += 1 ;
                  return Buildup_Tree( temp_node, current_token_id ) ;
                } // if 
                else if ( mlist_of_exp[current_token_id].mToken_TYPE == "LEFT_PAREN" )
                {
                  // temp_token = mlist_of_exp[current_token_id] ;
                  temp_node3 = InsertStructNode( temp_node, "RIGHT" ) ;
                  temp_node5 = CreateNilNode() ; 
                  temp_node5->mparent = temp_node3 ;
                  temp_node3->mright = temp_node5 ;
                  temp_node3->mparent = temp_node ; 
                  // current_token_id += 1 ;

                  return Buildup_Tree( temp_node3, current_token_id ) ;
                } // else if 
              } // if 

              return Buildup_Tree( temp_node, current_token_id ) ;
            } // if 
          } // else if 
          else 
          {
            // 不知道為何加
            return Buildup_Tree( temp_node, current_token_id ) ;
          } // else 

        } // if
        else 
        {
          // 不知道為何加
          return Buildup_Tree( temp_node, current_token_id ) ;
        } // else 
      } // else if 
      else // 沒DOT 規則2
      {
        // cout << "Here is right without DOT\n" ;
        temp_node = InsertStructNode( current_node, "RIGHT" ) ;
        if ( current_token_id == -1 )
        {
          current_token_id = mlist_of_exp.size() -1 ;
        } // if 
        /*
        string temp_node2_struct_type ;
        temp_node2 = current_node->mleft;
        temp_node2_struct_type = temp_node2->mstruct_type ;
        if ( temp_node2_struct_type == "QUOTE" )
        {
          if ( current_token_id ==  -1 )
          {
            current_token_id = mlist_of_exp.size() ;
          } // if 
        } // if 
        */

        return Buildup_Tree( temp_node, current_token_id ) ;
      } // else 
    } // else if 
    else // 左右都有野 
    {
      // cout << "Both way are full\nBack to parent\n" ;
     
      if ( temp_token.mToken_TYPE == "RIGHT_PAREN" )
      {
        if ( current_node->mleft->mstruct_type == "INFORMATION" 
             || current_node->mleft->mstruct_type == "NIL" )
        {
          if ( current_node->mright->mstruct_type == "INFORMATION" 
               || current_node->mright->mstruct_type == "NIL" ) 
          {
            if ( current_node->mparent != NULL )
            {
              temp_node = current_node->mparent ;
              if ( temp_node->mstruct_type == "STRUCT" && temp_node->mleft->mstruct_type == "QUOTE" )
              {
                current_token_id -= 1 ;
              } // if 
            } // if 
            
            if ( current_token_id != -1 )
            {
              current_token_id += 1 ;
            } // if 
          } // if
          else 
          {

          } // else 
        } // if 
      } // if 

      return Buildup_Tree( current_node->mparent, current_token_id ) ;
    } // else 
    // ********************************************************
    
    return Buildup_Tree( current_node->mparent, current_token_id ) ;
  } // Buildup_Tree()

  Node* Is_It_Exit( Node* node )
  {
    string error_message ; 
    if ( node->mparent == NULL )
    {
      return node ;
    } // if 

    if ( node->mstruct_type == "STRUCT" )
    {
      if ( node->mleft != NULL )
      {
        if ( node->mleft->minformation.Token_ID() == "exit" )
        {
          if ( node->mright != NULL )
          {
            if ( node->mright->mstruct_type == "NIL" )
            {
              error_message = "(exit)";
              throw error_message ;
            } // if 
          } // if 
        } // if 
      } // if 
    } // if 
    
    return node ;
  } // Is_It_Exit()

  Node* PrettyPrint( Node* node, int &space_counter ) 
  {
    // Recurrsion 的底
    if ( node->mparent == NULL ) 
    {
      return node ;
    } // if 

    stringstream stoFloat ;
    bool is_left_printed = false ;
    bool is_right_printed = false ;
    int size_of_string = 0 ;
    float num_float = 0.0 ;
    Node* temp ;
    Node* temp2 ;

    if ( node->mstruct_type == "STRUCT" )
    {
      if ( node->mparent != NULL )
      {
        temp = node->mparent ;
        if ( temp->mparent != NULL )
        {
          // STRUCT的PARENT有PARENT
          temp2 = temp->mparent ;
          
          if ( temp2->mleft == temp )
          {
            // STRUCT的PARENT是其PARENT的左邊
            if ( temp->mleft == node )
            {
              // 是左邊的STRUCT
              cout << "( " ;
              space_counter += 2 ;
            } // if 
            else if ( temp->mright == node && node->mleft->mstruct_type == "QUOTE" )
            {
              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for
              
      
              cout << "( " ;
              space_counter += 2 ;
            } // else if 
            else 
            {
              // 甚麼都不做
            } // else 
          } // if 
          else 
          {
            // STRUCT的PARENT是其PARENT的右邊
            if ( temp->mleft == node )
            {
              // 是左邊的STRUCT
              
              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for
              
      
              cout << "( " ;
              space_counter += 2 ;
            } // if 
            else if ( temp->mright == node && node->mleft->mstruct_type == "QUOTE" )
            {
              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for
              
      
              cout << "( " ;
              space_counter += 2 ;
            } // else if 
          } // else
        } // if 
        else 
        {
          // STRUCT的PARENT是ROOT
          if ( temp->mleft == node )
          {
            // 是左邊的STRUCT
            cout << "( " ;
            space_counter += 2 ;
          } // if 
        } // else 
      } // if 

      if ( node->mleft != NULL ) 
      {
        if ( is_left_printed == true )
        {

          if ( node->mright != NULL )
          {
            PrettyPrint( node->mright, space_counter ) ;
          } // if 
          else 
          {
            PrettyPrint( node->mparent, space_counter ) ;
          } // else 
        } // if 
        else 
        {
          PrettyPrint( node->mleft, space_counter ) ;
          is_left_printed = true ;
        } // else 
      } // if 
      
      if ( node->mright != NULL )
      {
        if ( is_right_printed == true )
        {
          PrettyPrint( node->mparent, space_counter ) ;
        } // if
        else 
        {
          PrettyPrint( node->mright, space_counter ) ;
          is_right_printed = true ;
        } // else 
      } // if 
      else 
      {
        space_counter = space_counter - 2 ;
        for ( int i = 0 ; i < space_counter ; i++ )
        {
          cout << " " ;
        } // for

        cout << ")\n" ;
        is_right_printed = true ;
      } // else 
    } // if
    else if ( node->mstruct_type == "QUOTE" )
    {
      /*
      for ( int i = 0 ; i < space_counter ; i++ )
      {
        cout << " " ;
      } // for
      */

      cout << "quote\n" ;
      // space_counter += 2 ;

      return node->mparent ;
    } // else if 
    else if ( node->mstruct_type == "INFORMATION" )
    {  
      if ( node->minformation.mToken_TYPE == "INT" || node->minformation.mToken_TYPE == "FLOAT" 
           || node->minformation.mToken_TYPE == "SYMBOL" || node->minformation.mToken_TYPE == "STRING"
           || node->minformation.mToken_TYPE == "T" )
      {
        if ( node->mparent != NULL )
        {
          // "INFORMATION"有Parent
          temp = node->mparent ;
          if ( temp->mparent != NULL )
          {
            // "INFORMATION"的Parent有Parent

            temp2 = temp->mparent ;
            if ( temp2->mright != NULL )
            {
              // "INFORMATION"的Parent的Parent 右邊不是空
              if ( temp2->mright == temp )
              {
                // 証明"INFORMATION"的Parent是個右邊的STRUCT
                // 要印空白
                if ( temp->mright == node )
                {
                  // 証明"INFORMATION"是在其Parent的右邊
                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for

                  cout << ".\n" ;

                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for

                  if ( node->minformation.mToken_TYPE == "SYMBOL" )
                  {
                    cout << node->minformation.Token_ID() << "\n" ;
                  } // if 
                  else if ( node->minformation.mToken_TYPE == "STRING" )
                  {
                    Print_String( node->minformation ) ;
                  } // else if 
                  else if ( node->minformation.mToken_TYPE == "T" )
                  {
                    cout << "#t" << "\n" ;
                  } // else if 
                  else 
                  {
                    if ( node->minformation.mToken_TYPE == "INT" )
                    {
                      cout << Get_String_Value( node->minformation ) << "\n" ;
                    } // if 
                    else 
                    {
                      stoFloat << Get_String_Value( node->minformation ) ;
                      stoFloat >> num_float ;
                      printf( "%.3f\n", num_float ) ;
                    } // else 
                  } // else 

                  space_counter = space_counter - 2 ;

                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for

                  cout << ")\n" ;
                } // if 
                else 
                {
                  
                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for
                  

                  if ( node->minformation.mToken_TYPE == "SYMBOL" )
                  {
                    cout << node->minformation.Token_ID() << "\n" ;
                  } // if 
                  else if ( node->minformation.mToken_TYPE == "STRING" )
                  {
                    Print_String( node->minformation ) ;
                  } // else if 
                  else if ( node->minformation.mToken_TYPE == "T" )
                  {
                    cout << "#t" << "\n" ;
                  } // else if 
                  else 
                  {
                    if ( node->minformation.mToken_TYPE == "INT" )
                    {
                      cout << Get_String_Value( node->minformation ) << "\n" ;
                    } // if 
                    else 
                    {
                      stoFloat << Get_String_Value( node->minformation ) ;
                      stoFloat >> num_float ;
                      printf( "%.3f\n", num_float ) ;
                    } // else 
                  } // else 
                } // else 
              } // if 
              else 
              {
                // 証明"INFORMATION"的Parent是個左邊的STRUCT
                if ( temp->mright == node )
                {
                  // 証明"INFORMATION"是在其Parent的右邊
                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for

                  cout << ".\n" ;

                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for

                  if ( node->minformation.mToken_TYPE == "SYMBOL" )
                  {
                    cout << node->minformation.Token_ID() << "\n" ;
                  } // if 
                  else if ( node->minformation.mToken_TYPE == "STRING" )
                  {
                    Print_String( node->minformation ) ;
                  } // else if 
                  else if ( node->minformation.mToken_TYPE == "T" )
                  {
                    cout << "#t" << "\n" ;
                  } // else if 
                  else 
                  {
                    if ( node->minformation.mToken_TYPE == "INT" )
                    {
                      cout << Get_String_Value( node->minformation ) << "\n" ;
                    } // if 
                    else 
                    {
                      stoFloat << Get_String_Value( node->minformation ) ;
                      stoFloat >> num_float ;
                      printf( "%.3f\n", num_float ) ;
                    } // else 
                    
                  } // else 

                  space_counter = space_counter - 2 ;

                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for

                  cout << ")\n" ;
                } // if 
                else 
                {
                  /*
                  for ( int i = 0 ; i < space_counter ; i++ )
                  {
                    cout << " " ;
                  } // for
                  */

                  if ( node->minformation.mToken_TYPE == "SYMBOL" )
                  {
                    cout << node->minformation.Token_ID() << "\n" ;
                  } // if 
                  else if ( node->minformation.mToken_TYPE == "STRING" )
                  {
                    Print_String( node->minformation ) ;
                  } // else if 
                  else if ( node->minformation.mToken_TYPE == "T" )
                  {
                    cout << "#t" << "\n" ;
                  } // else if 
                  else 
                  {
                    if ( node->minformation.mToken_TYPE == "INT" )
                    {
                      cout << Get_String_Value( node->minformation ) << "\n" ;
                    } // if 
                    else 
                    {
                      stoFloat << Get_String_Value( node->minformation ) ;
                      stoFloat >> num_float ;
                      printf( "%.3f\n", num_float ) ;
                    } // else 
                  } // else 
                } // else 

              } // else  
              
          
            } // if 
            else if ( temp->mright == node )
            {
              // 証明"INFORMATION"是在Parent的右邊
              // 是 X . Y 中的Y
              // 要印空白
              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for

              cout << ".\n" ;

              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for

              if ( node->minformation.mToken_TYPE == "SYMBOL" )
              {
                cout << node->minformation.Token_ID() << "\n" ;
              } // if 
              else if ( node->minformation.mToken_TYPE == "STRING" )
              {
                Print_String( node->minformation ) ;
              } // else if 
              else if ( node->minformation.mToken_TYPE == "T" )
              {
                cout << "#t" << "\n" ;
              } // else if 
              else 
              {    
                if ( node->minformation.mToken_TYPE == "INT" )
                {
                  cout << Get_String_Value( node->minformation ) << "\n" ;
                } // if 
                else 
                {
                  stoFloat << Get_String_Value( node->minformation ) ;
                  stoFloat >> num_float ;
                  printf( "%.3f\n", num_float ) ;
                } // else 
              } // else 

              space_counter = space_counter - 2 ;

              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for

              cout << ")\n" ;
            } // else if 
            else 
            {
              // 証明"INFORMATION"是在Parent的左邊
              // 是 X . Y 中的X
              if ( node->minformation.mToken_TYPE == "SYMBOL" )
              {
                cout << node->minformation.Token_ID() << "\n" ;
              } // if 
              else if ( node->minformation.mToken_TYPE == "STRING" )
              {
                Print_String( node->minformation ) ;
              } // else if 
              else if ( node->minformation.mToken_TYPE == "T" )
              {
                cout << "#t" << "\n" ;
              } // else if 
              else 
              {
                if ( node->minformation.mToken_TYPE == "INT" )
                {
                  cout << Get_String_Value( node->minformation ) << "\n" ;
                } // if 
                else 
                {
                  stoFloat << Get_String_Value( node->minformation ) ;
                  stoFloat >> num_float ;
                  printf( "%.3f\n", num_float ) ;
                } // else 
              } // else 
            } // else 
          } // if 
          else 
          {
            // "INFORMATION"的STRUCT沒有Parent
            // test use
            if ( temp->mright == node )
            {
              // 証明"INFORMATION"是在Parent的右邊
              // 是 X . Y 中的Y
              // 要印空白
              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for

              cout << ".\n" ;

              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for

              if ( node->minformation.mToken_TYPE == "SYMBOL" )
              {
                cout << node->minformation.Token_ID() << "\n" ;
              } // if 
              else if ( node->minformation.mToken_TYPE == "STRING" )
              {
                Print_String( node->minformation ) ;
              } // else if 
              else if ( node->minformation.mToken_TYPE == "T" )
              {
                cout << "#t" << "\n" ;
              } // else if 
              else 
              {
                if ( node->minformation.mToken_TYPE == "INT" )
                {
                  cout << Get_String_Value( node->minformation ) << "\n" ;
                } // if 
                else 
                {
                  stoFloat << Get_String_Value( node->minformation ) ;
                  stoFloat >> num_float ;
                  printf( "%.3f\n", num_float ) ;
                } // else 
              } // else  

              space_counter = space_counter - 2 ;

              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for

              cout << ")\n" ;
            } // if
            else 
            {
              // 証明"INFORMATION"是在Parent的左邊
              // 是 X . Y 中的X
              
              if ( node->minformation.mToken_TYPE == "SYMBOL" )
              {
                cout << node->minformation.Token_ID() << "\n" ;
              } // if 
              else if ( node->minformation.mToken_TYPE == "STRING" )
              {
                Print_String( node->minformation ) ;
              } // else if 
              else if ( node->minformation.mToken_TYPE == "T" )
              {
                cout << "#t" << "\n" ;
              } // else if 
              else 
              {
                if ( node->minformation.mToken_TYPE == "INT" )
                {
                  cout << Get_String_Value( node->minformation ) << "\n" ;
                } // if 
                else 
                {
                  stoFloat << Get_String_Value( node->minformation ) ;
                  stoFloat >> num_float ;
                  printf( "%.3f\n", num_float ) ;
                } // else 
              } // else 
            } // else 
          } // else 
        } // if 
        else 
        {
          // 沒有Parent直接印
          if ( node->minformation.mToken_TYPE == "SYMBOL" )
          {
            cout << node->minformation.Token_ID() << "\n" ;
          } // if 
          else if ( node->minformation.mToken_TYPE == "STRING" )
          {
            size_of_string = node->minformation.mToken.size() ;

            for ( int i = 0 ; i < size_of_string ; i++ )
            {
              char temp_char = node->minformation.mToken[i].mToken_Char ;
              cout << temp_char ;
            } // for

            cout << endl ;
          } // else if 
          else 
          {
            // 是INT/FLOAT
            if ( node->minformation.mToken_TYPE == "INT" )
            {
              cout << Get_String_Value( node->minformation ) << "\n" ;
            } // if 
            else 
            {
              stoFloat << Get_String_Value( node->minformation ) ;
              stoFloat >> num_float ;
              printf( "%.3f\n", num_float ) ;
            } // else 
          } // else 
        } // else 

      } // if 
      
      
      return node->mparent ;
    } // else if 
    else if ( node->mstruct_type == "NIL" )
    {
      // cout << "NIL\n" ;
      temp = node->mparent ;
      if ( temp->mparent != NULL )
      {
        temp2 = temp->mparent ;
        if ( temp2->mright != NULL )
        {
          // "NIL"的Parent的Parent 右邊不是空
          if ( temp2->mright == temp )
          {
            // 証明"INFORMATION"的Parent是個右邊的STRUCT
            // 要印空白
            if ( temp->mleft == node )
            { 
              
              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for
              

              cout << "nil\n" ;
            } // if
            else 
            {
              if ( temp->mright == node )
              {
                space_counter = space_counter - 2 ;
                for ( int i = 0 ; i < space_counter ; i++ )
                {
                  cout << " " ;
                } // for
              } // if 

              cout << ")\n" ;
            } // else 
          } // if  
          else 
          {
            if ( temp->mleft == node )
            { 
              cout << "nil\n" ;
            } // if
            else 
            {
              if ( temp->mright == node )
              {
                space_counter = space_counter - 2 ;
                for ( int i = 0 ; i < space_counter ; i++ )
                {
                  cout << " " ;
                } // for
              } // if 

              cout << ")\n" ;
            } // else 
          } // else 
        } // if 
        else 
        {
          // "NIL"的Parent的Parent 右邊是空
          if ( temp->mleft == node )
          { 
            cout << "nil\n" ;
          } // if
          else 
          {
            if ( temp->mright == node )
            {
              space_counter = space_counter - 2 ;
              for ( int i = 0 ; i < space_counter ; i++ )
              {
                cout << " " ;
              } // for
            } // if 

            cout << ")\n" ;
          } // else 
        } // else 
      } // if 
      else if ( temp->mleft == node )
      { 
        /*
        for ( int i = 0 ; i < space_counter ; i++ )
        {
          cout << " " ;
        } // for
        */

        cout << "nil\n" ;
      } // if
      else 
      {
        if ( temp->mright == node )
        {
          space_counter = space_counter - 2 ;
          for ( int i = 0 ; i < space_counter ; i++ )
          {
            cout << " " ;
          } // for
        } // if 

        cout << ")\n" ;
      } // else 

      return node->mparent ;
    } // else if 

    if ( is_left_printed == true && is_right_printed == true )
    {
      if ( node->mparent == NULL || node->mparent->mparent == NULL )
      {
        return node->mparent ;
      } // if 
      else if ( node->mleft->mstruct_type == "QUOTE" )
      {
        if ( node->mparent->mright == node ) 
        {
          space_counter = space_counter - 2 ;
          for ( int i = 0 ; i < space_counter ; i++ )
          {
            cout << " " ;
          } // for

          cout << ")\n" ;
        } // if 
        /*
        space_counter = space_counter - 2 ;
        for ( int i = 0 ; i < space_counter ; i++ )
        {
          cout << " " ;
        } // for

        cout << ")\n" ;
        */

        return node->mparent ;
      } // else if 
      else 
      {
        return node->mparent ;
      } // else
    } // if 
    else 
    {
      return node->mparent ;
    } // else 
  } // PrettyPrint()
  // *******************************************************
}; // Parser

// ********************************************************************************************

int main()
{
  bool end_Of_Input = false;
  Parser project;
  project.mscanner.mCurrent_Line = 1;
  project.mscanner.mCurrent_Column = 0;
  project.mscanner.mScanner_Token_Column = 0;
  project.mscanner.mScanner_Token_Line = 1;
  char uTestNum;
  uTestNum = cin.get();
  uTestNum = cin.get();
  cout << "Welcome to OurScheme!\n";
  string test;
  Form_Of_Token input_token ;
  cout << "\n" ;
  cout << "> ";
  while ( end_Of_Input != true )
  {
    try
    {
      Node* root ;
      int token_id = 0 ;
      int space_counter = 0 ;
      bool is_left_printed = false ;
      bool is_right_printed = false ;
      bool have_char = false ;
      bool command_counter = false ;
      project.mdot_Right_Parent = false ; 
      project.mRight_Parent_safety_buffer = false ; 
      char waste ;
      root = project.CreateStructNode() ;
      root->mparent = NULL ;
      // cout << "Yo1:" << project.mscanner.mCurrent_Line << endl;
      // project.mdot_Right_Parent = false ;
      project.ReadSExp( input_token ) ;
      /*
      for( int i = 0 ; i < project.mlist_of_exp.size() ; i++ )
      {
        cout << project.mlist_of_exp[i].Token_ID() << "  " << project.mlist_of_exp[i].mToken_TYPE << endl ;

      } // for
      */
      /*
      for ( int i = 0 ; i < project.mlist_of_exp.size() ; i++ )
      {
        for ( int u = 0 ; u < project.mlist_of_exp[i].mToken.size() ; u++ )
        {
          cout << "Token char : " << project.mlist_of_exp[i].mToken[u].mToken_Char << endl ;
          cout << "Token line : " << project.mlist_of_exp[i].mToken[u].mToken_LINE << endl ;
          cout << "Token column : " << project.mlist_of_exp[i].mToken[u].mToken_COLUMN << endl ;
        } // for 
      } // for 
      */

      root = project.Buildup_Tree( root, token_id ) ;
      // cout << "yo " << root->mleft->mright->mleft->minformation.Token_ID() ; 
      project.Is_It_Exit( root->mleft );
      project.PrettyPrint( root->mleft, space_counter ) ;
      cout << "\n";
      cout << "> ";
      project.mscanner.mCurrent_Line = 1;
      project.mscanner.mCurrent_Column = 0;
      
      if ( !cin.eof() )
      {
        while ( have_char != true )
        {
          if ( !cin.eof() )
          {
            if ( cin.peek() == ' ' || cin.peek() == '\t' )
            {
              project.mscanner.mCurrent_Column += 1 ;
              waste = cin.get() ;
            } // if 
            else if ( cin.peek() == '\n' )
            {
              waste = cin.get() ;
              project.mscanner.mCurrent_Column = 0;
              project.mscanner.mScanner_Token_Column = 0 ;
              have_char = true ;
            } // else if 
            else if ( cin.peek() == ';' )
            {
              // Clear_line 和 CommandReader 
              // 分別在Clear_line的mCurrent_line會設為1
              // CommandReader 的mCurrent_line會設為疊加
              if ( command_counter == false )
              {
                project.mscanner.Clear_line() ; 
                command_counter = true ;
              } // if 
              else 
              {
                project.mscanner.CommanReader() ;
              } // else 
            } // else if 
            else 
            {
              have_char = true ;
            } // else 
          } // if 
          else
          {
            have_char = true ;
          } // else 
        } // while 

      } // if 
    } // try
    catch ( string error_message )
    {
      project.mscanner.mCurrent_Line = 1;
      project.mscanner.mCurrent_Column = 0;
      project.mscanner.mScanner_Token_Column = 0;
      project.mscanner.mScanner_Token_Line = 1;
      project.mscanner.mBuffer_Token.clear();
      if ( error_message == "(exit)" || error_message == "ERROR (no more input) : END-OF-FILE encountered" )
      {
        end_Of_Input = true ;
      } // if
      else
      {
        project.mscanner.Clear_line();
        cout << error_message << endl;
        cout << "\n> ";
      } // else 

      if ( error_message == "ERROR (no more input) : END-OF-FILE encountered" )
      {
        cout << error_message ;
      } // if 
    } // catch 

  } // while
    

  cout << "\nThanks for using OurScheme!";
} // main()


