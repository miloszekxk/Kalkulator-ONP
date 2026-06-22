#include <iostream>
#include <string>
#include <math.h>
using namespace std;

/*
 Kalkulator wyrazen w Odwrotnej Notacji Polskiej (ONP).
 Obsluguje liczby wielocyfrowe i zmiennoprzecinkowe (np. 212, 3.14),
 operatory + - * / ^ oraz nawiasy.
 Kroki: walidacja -> tablica wag -> konwersja na ONP -> obliczenie.
 */

const int MAX_ZNAKOW = 100;
const int PRIORYTET_NAWIASU = 10;

// STOS NA OPERATORY (uzywany przy konwersji na ONP)
char stosOp[MAX_ZNAKOW];        // tablica na operatory i nawiasy
int plywakOp = -1;              // indeks ostatniego elementu, -1 = pusto

void naStosOp(char znak){
    plywakOp++;
    stosOp[plywakOp] = znak;
}

char zeStosuOp(){
    char znak = stosOp[plywakOp];
    plywakOp--;
    return znak;
}

char coNaGorzeOp(){
    return stosOp[plywakOp];
}

bool czyPustyOp(){
    return plywakOp == -1;
}

// STOS NA LICZBY (uzywany przy obliczaniu ONP)
double stos[MAX_ZNAKOW];        //tablica na operandy (liczby)
int plywak = -1;

// Liczby na stos operandow
void naStos(double liczba){
    plywak++;
    stos[plywak] = liczba;
}

double zeStosu(){
    double liczba = stos[plywak];
    plywak--;
    return liczba;
}

bool czyCyfa(char c){
    return c >= '0' && c <= '9';
}

bool czyOperator(char c){
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

int priorytet(char c){
    if (c == '+' || c == '-') return 1;
    if (c == '*' || c == '/') return 2;
    if (c == '^') return 3;
    return 0;
}

// Zamiana napisu na liczbe
double tekstNaLiczbe(string tekst){
    double wynik = 0.0;
    int i = 0;
    int n = tekst.length();
    // czesc calkowita 
    while (i < n && tekst[i] != '.'){
        wynik = wynik * 10 + (tekst[i] - '0');
        i++;
    }
    // czesc ulamkowa
    if (i < n && tekst[i] == '.'){
        i++;
        double mnoznik = 0.1;
        while (i < n){
            wynik += (tekst[i] - '0') * mnoznik;
            mnoznik /= 10;
            i++;
        }
    }
    return wynik;
}

// Tablica wag: dla operatora priorytet + podbicie za nawiasy, dla liczby 0.
void wypiszWagi(string rownanie){
    int podbicie = 0;
    int n = rownanie.length();
    for (int i = 0; i < n; i++){
        char c = rownanie[i];
        if (c == '(') podbicie += PRIORYTET_NAWIASU;
        else if (c == ')') podbicie -= PRIORYTET_NAWIASU;
        else if (czyOperator(c)) cout << priorytet(c) + podbicie << " | ";
        else cout << 0 << " | ";        // cyfra i kropka maja wage 0
    }
    cout << endl;
}

/* WALIDACJA
 Sprawdza dozwolone znaki, bilans nawiasow oraz skladnie - na przemian
 oczekujemy operandu (liczba lub '(') i operatora. Liczba to ciag cyfr
 z co najwyzej jedna kropka dziesietna.
 */
bool czyPoprawne(string rownanie){
    if (rownanie.empty() || (int)rownanie.length() > MAX_ZNAKOW) return false;

    int otwarte = 0;
    bool oczekujOperand = true;   // na starcie spodziewamy sie operandu (liczby lub '(')
    int i = 0;
    int n = rownanie.length();

    while (i < n){
        char c = rownanie[i];

        if (c == '('){
            if (!oczekujOperand) return false;
            otwarte++;
            i++;
        }
        else if (c == ')'){
            if (oczekujOperand) return false;
            otwarte--;
            if (otwarte < 0) return false;
            i++;
        }
        else if (czyOperator(c)){
            if (oczekujOperand) return false;  // np. "*2" lub "(+3)"
            oczekujOperand = true;             // po operatorze spodziewamy sie operandu
            i++;
        }
        else if (czyCyfa(c) || c == '.'){
            if (!oczekujOperand) return false; //liczba tam gdzie spodziewamy sie operatora
            // wczytaj cala liczbe: ciag cyfr z co najwyzej jedna kropka
            bool kropka = false;
            int cyfry = 0;
            while (i < n && (czyCyfa(rownanie[i]) || rownanie[i] == '.')){
                if (rownanie[i] == '.'){
                    if (kropka) return false;   // druga kropka w liczbie
                    kropka = true;
                } else {
                    cyfry++;
                }
                i++;
            }
            if (cyfry == 0) return false;       // np. sama kropka "."
            oczekujOperand = false;
        }
        else return false;                      // niedozwolony znak
    }
    return otwarte == 0 && !oczekujOperand;
}

/* KONWERSJA: SHUNTING-YARD
 Liczby (cale tokeny) ida na wyjscie. Operatory na stos - przed odlozeniem
 zdejmujemy operatory o wyzszym (lub rownym dla lewostronnych) priorytecie.
 Nawias '(' odkladamy, przy ')' zdejmujemy az do '('.
 */
string naONP(string rownanie){
    string onp = "";        // wynik w onp (tokeny rozdzielone spacja)
    int i = 0;
    int n = rownanie.length();

    while (i < n){
        char c = rownanie[i];

        if (czyCyfa(c) || c == '.'){
            while (i < n && (czyCyfa(rownanie[i]) || rownanie[i] == '.')){
                onp += rownanie[i];
                i++;
            }
            onp += ' ';     //spacja konczy token liczby
        }
        else if (c == '('){
            naStosOp('(');      //nawias otwierajacy na stos
            i++;
        }
        else if (c == ')'){
            while (!czyPustyOp() && coNaGorzeOp() != '('){  //zdejmuje ze szczytu stosu az do '('
                onp += zeStosuOp();
                onp += ' ';
            }
            zeStosuOp();   // usuwamy '('
            i++;
        }
        else{   // operator, zdejmuje ze szczytu stosu operatory, ktore maja pierwszenstwo nad biezacym
            while (!czyPustyOp() && coNaGorzeOp() != '('){
                char gora = coNaGorzeOp();
                bool zdejmij = false;
                if (priorytet(gora) > priorytet(c)) zdejmij = true;                 //wyzszy priorytet
                if (priorytet(gora) == priorytet(c) && c != '^') zdejmij = true;    //rowny priorytet i lewostronny
                                                                                    // (c == '^' - nie zdejmuje bo prawostronny)
                if (!zdejmij) break;
                onp += zeStosuOp();
                onp += ' ';
            }
            naStosOp(c);        //biezacy operator na stos
            i++;
        }
    }
    // po przejsciu calego rownania zrzucamy pozostale operatory na wyjscie
    while (!czyPustyOp()){
        onp += zeStosuOp();
        onp += ' ';
    }
    return onp;
}

/* EWALUACJA ONP
 Tokeny rozdzielone spacja. Liczbe odkladamy na stos. Operator zdejmuje dwa
 operandy, liczy i odklada wynik. Dzielenie przez zero ustawia flage bledu.
 */
double obliczONP(string onp, bool& blad){
    string token = "";   // budowany pojedynczy token
    int n = onp.length();

    for (int i = 0; i <= n; i++){
        char c = ' ';
        if (i < n) c = onp[i];   // na koncu sztuczna spacja domyka ostatni token

        if (c == ' '){
            if (token.empty()) continue;
            // token dlugosci 1 i jest operatorem wykonaj dzialanie
            if (token.length() == 1 && czyOperator(token[0])){
                double b = zeStosu();               // prawy operand zdjety jako pierwszy
                double a = zeStosu();               // lewy operand zdjety jako drugi
                char op = token[0];
                if (op == '+') naStos(a + b);
                else if (op == '-') naStos(a - b);
                else if (op == '*') naStos(a * b);
                else if (op == '/'){
                    if (b == 0) {blad = true; naStos(0);}        // wykrycie dzielenia przez zero przed wykonaniem operacji
                    else naStos(a / b);
                }
                else if (op == '^') naStos(pow(a, b));
            }
            else{
                naStos(tekstNaLiczbe(token)); // token to liczba, odkladamy na stos
            }
            token = "";  // reset przed kolejnym tokenem
        }
        else{
            token += c;  //dodajemy znak do biezacego tokenu
        }
    }
    return zeStosu();  // na stosie zostaje jeden element - wynik koncowy
}

int main(){
    string rownanie;
    cout << "Podaj rownanie: ";
    cin >> rownanie;

    if (!czyPoprawne(rownanie)){
        cout << "Blad: niepoprawne wyrazenie!" << endl;
        return 1;
    }

    wypiszWagi(rownanie);

    string onp = naONP(rownanie);
    cout << "Postac ONP: " << onp << endl;

    bool blad = false;
    double wynik = obliczONP(onp, blad);

    if (blad)
        cout << "Blad: dzielenie przez zero!" << endl;
    else
        cout << "Wynik: " << wynik << endl;

    return 0;
}
