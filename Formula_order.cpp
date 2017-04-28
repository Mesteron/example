public class FormulaOrder {
    private static int time = 1;
    private static ArrayList<String> result = new ArrayList<>();
 
    public static void main(String[] args) throws SyntaxError {
        Parser parser = new Parser();
        try {
            Graph graph = parser
import java.util.function.IntPredicate;
import java.util.*;
 
class Position {
    String text;
    int index;
 
    public Position(String text) {
        this(text, 0);
    }
 
    private Position(String text, int index) {
        this.text = text;
        this.index = index;
    }
 
    public int getChar() {
        return index < text.length() ? text.codePointAt(index) : -1;
    }
 
    public boolean satisfies(IntPredicate p) {
        return p.test(getChar());
    }
 
    public Position skip() {
        int c = getChar();
        switch (c) {
            case -1:
                return this;
            default:
                return new Position(text, index + 1);
        }
    }
 
    public Position skipWhile(IntPredicate p) {
        Position pos = this;
        while (pos.satisfies(p)) pos = pos.skip();
        return pos;
    }
}
 
class SyntaxError extends Exception {
    public SyntaxError(String msg) {
        super(String.format(msg));
    }
}
 
enum Tag {
    IDENT,
    NUMBER,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    MUL,
    DIV,
    EQUAL,
    COM,
    END_OF_TEXT;
 
    public String toString() {
        switch (this) {
            case IDENT: return "identifier";
            case NUMBER: return "number";
            case PLUS: return "'+'";
            case MINUS: return "'-'";
            case MUL: return "'*'";
            case DIV: return "'/'";
            case LPAREN: return "'('";
            case RPAREN: return "')'";
            case EQUAL: return "'='";
            case END_OF_TEXT: return "end of text";
            case COM: return "',";
        }
        throw new RuntimeException("error");
    }
}
 
class Token {
    private Tag tag;
    Position start, follow;
    public String var,text;
 
    public Token(String text) throws SyntaxError {
        this(new Position(text),text);
    }
 
    private Token(Position cur,String text) throws SyntaxError {
        this.text = text;
        start = cur.skipWhile(Character::isWhitespace);
        follow = start.skip();
        switch (start.getChar()) {
            case -1:
                tag = Tag.END_OF_TEXT;
                break;
            case '(':
                tag = Tag.LPAREN;
                break;
            case ')':
                tag = Tag.RPAREN;
                break;
            case '+':
                tag = Tag.PLUS;
                break;
            case '-':
                tag = Tag.MINUS;
                break;
            case '*':
                tag = Tag.MUL;
                break;
            case '/':
                tag = Tag.DIV;
                break;
            case '=':
                tag = Tag.EQUAL;
                break;
            case ',':
                tag = Tag.COM;
                break;
            default:
                if (start.satisfies(Character::isLetter)) {
                    follow = follow.skipWhile(Character::isLetterOrDigit);
                    var = text.substring(start.index,follow.index);
                    tag = Tag.IDENT;
                } else if (start.satisfies(Character::isDigit)) {
                    follow = follow.skipWhile(Character::isDigit);
                    if (follow.satisfies(Character::isLetter)) {
                        throw new SyntaxError("syntax error");
                    }
                    tag = Tag.NUMBER;
                } else {
                    throwError("syntax error");
                }
        }
    }
 
    public void throwError(String msg) throws SyntaxError {
        throw new SyntaxError(msg);
    }
 
    public boolean matches(Tag ...tags) {
        return Arrays.stream(tags).anyMatch(t -> tag == t);
    }
 
    public Token next() throws SyntaxError {
        return new Token(follow,text);
    }
}
 
class Graph {
    public ArrayList<Vertex> vertices = new ArrayList<>();
    public HashMap<Vertex,String> formul_map = new HashMap<>();
    public HashMap<String, Vertex> vertex = new HashMap<>();
}
 
class Vertex {
    public ArrayList<String> var;
    public HashSet<String> bound = new HashSet<>();
    public int t1,t2;
 
    public Vertex(ArrayList<String> var) {
        this.var = (ArrayList<String>) var.clone();
        t1 = t2 = 0;
    }
 
}
 
class Parser {
    private Token sym;
    private ArrayList<String> formuls = new ArrayList<>();
    private int cur, count;
    private HashSet<String> calls = new HashSet<>();
    private HashSet<String> cur_calls = new HashSet<>();
    private Graph graph = new Graph();
 
    private void expect(Tag tag) throws SyntaxError {
        if (!sym.matches(tag)) {
            sym.throwError("syntax error");
        }
        sym = sym.next();
    }
 
     public Graph parsing() throws SyntaxError{
        Scanner in = new Scanner(System.in);
        cur = 0;
        while(in.hasNext()) {
            String text = in.nextLine();
            sym = new Token(text);
            parseFormula(text);
            cur++;
        }
        for(String call : calls) {
            if(!formuls.contains(call)) sym.throwError("syntax error");
        }
        expect(Tag.END_OF_TEXT);
 
        return graph;
    }
    private void parseFormula(String text) throws SyntaxError {
        if(sym.matches(Tag.IDENT)) {
            count = 0;
            ArrayList<String> cur_formula = new ArrayList<>();
            parseIdentList(cur_formula);
            Vertex vertex = new Vertex(cur_formula);
            for(String s : cur_formula) graph.vertex.put(s,vertex);
            graph.vertices.add(vertex);
            graph.formul_map.put(vertex, text);
            expect(Tag.EQUAL);
            cur_calls.clear();
            parseExprList();
            if(cur_formula.size() != count+1) 								sym.throwError("syntaxerror");
            vertex.bound.addAll(cur_calls);
        }
    }
 
    private void parseIdentList(ArrayList<String> cur_formula) 
			throws SyntaxError {
        if(sym.matches(Tag.IDENT)) {
            if(formuls.contains(sym.var)) sym.throwError("syntax error");
            cur_formula.add(sym.var);
            formuls.add(sym.var);
            sym = sym.next();
            parseIdentList2(cur_formula);
        }
        else {
            sym.throwError("syntax error");
        }
    }
 
    private void parseIdentList2(ArrayList<String> cur_formula) 
		throws SyntaxError {
        if(sym.matches(Tag.COM)) {
            sym = sym.next();
            parseIdentList(cur_formula);
        }
    }
 
    private void parseExprList() throws SyntaxError {
        if(sym.matches(Tag.IDENT,Tag.NUMBER,Tag.MINUS,Tag.LPAREN)) {
            parseExpr();
            parseExprList2();
        } else {
            sym.throwError("syntax error");
        }
    }
 
    private void parseExprList2() throws SyntaxError {
        if(sym.matches(Tag.COM)) {
            sym = sym.next();
            count++;
            parseExprList();
        }
    }
    private  void parseExpr() throws SyntaxError {
        parseTerm();
        parseExpr2();
    }
    private void parseExpr2() throws SyntaxError {
        if (sym.matches(Tag.PLUS)) {
            sym = sym.next();
            parseTerm();
            parseExpr2();
        } else if (sym.matches(Tag.MINUS)) {
            sym = sym.next();
            parseTerm();
            parseExpr2();
        }
    }
 
    private void parseTerm() throws SyntaxError {
        parseFact();
        parseTerm2();
    }
 
    private void parseTerm2() throws SyntaxError {
        if (sym.matches(Tag.MUL)) {
            sym = sym.next();
            parseFact();
            parseTerm2();
        } else if (sym.matches(Tag.DIV)) {
            sym = sym.next();
            parseFact();
            parseTerm2();
        }
    }
 
    private void parseFact() throws SyntaxError {
        if (sym.matches(Tag.NUMBER)) {
            sym = sym.next();
        } else if (sym.matches(Tag.IDENT)) {
            cur_calls.add(sym.var);
            calls.add(sym.var);
            sym = sym.next();
        } else if (sym.matches(Tag.LPAREN)) {
            sym = sym.next();
            parseExpr();
            expect(Tag.RPAREN);
        } else if (sym.matches(Tag.MINUS)) {
            sym = sym.next();
            parseFact();
        } else {
            sym.throwError("syntax error");
        }
    }.parsing();
            dfs(graph);
            for(String s : result) System.out.println(s);
        } catch (SyntaxError e) {
            System.out.println(e.getMessage());
        }
    }
 
    public static void dfs(Graph graph) throws SyntaxError {
        for(Vertex v : graph.vertices)
            if(v.t1 == 0) visitVertex(graph, v);
    }
 
    public static void visitVertex(Graph graph, Vertex v) 
		throws SyntaxError {
        v.t1 = time++;
        for(String s : v.bound) {
            Vertex u = graph.vertex.get(s);
            if (u.t2 == 0 && u.t1!=0 ) throw new SyntaxError("cycle");
            if(u.t1 == 0) visitVertex(graph,u);
        }
        result.add(graph.formul_map.get(v));
        v.t2 = time++;
    }
}
