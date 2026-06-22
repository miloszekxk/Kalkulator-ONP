# Kalkulator wyrażeń arytmetycznych - Odwrotna Notacja Polska (ONP)

![C++](https://img.shields.io/badge/Język-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![g++](https://img.shields.io/badge/Kompilator-g%2B%2B%20(GCC)-4EAA25?logo=gnu&logoColor=white)
![Standard](https://img.shields.io/badge/Standard-C%2B%2B11%2B-00599C)
![Algorytm](https://img.shields.io/badge/Algorytm-Shunting--yard-FF6F00)
![Paradygmat](https://img.shields.io/badge/Paradygmat-strukturalny-555555)
![GitHub Markdown](https://img.shields.io/badge/Format-GitHub%20Markdown-181717?logo=github&logoColor=white)

---

## <img src="https://img.icons8.com/color/48/info--v1.png" alt="info" width="24" /> Opis projektu

Konsolowy kalkulator, który oblicza wyrażenia arytmetyczne podane w notacji infiksowej - np. jak `(2+1)*2-5^3`. Wyrażenie wpisywane jest jako
jeden ciąg znaków (bez spacji), a program wypisuje jego postać w Odwrotnej Notacji Polskiej oraz końcowy wynik.

Program nie liczy wyrażenia wprost - najpierw jest zamiana na **Odwrotną Notację
Polską (ONP)** za pomocą algorytmu **Shunting-yard**. W ONP nie ma nawiasów
ani niejednoznaczności kolejności działań, więc obliczenie sprowadza się do prostego przejścia
po tokenach: liczby odkładane są na stos, a operator zdejmuje dwa ostatnie operandy, wykonuje
działanie i odkłada wynik z powrotem.

Główną częścią jest **własna struktura stosu** zbudowana na tablicy z wskaźnikiem wierzchołka - bez korzystania z `std::stack` ani innych kontenerów STL. W programie występują **dwa niezależne stosy** o tej samej budowie: stos operatorów (elementy typu
`char`) używany podczas konwersji na ONP oraz stos operandów (elementy typu `double`)
używany podczas obliczania wyniku. Stos operatorów udostępnia komplet operacji
`push`, `pop`, `peek`, `isEmpty`, natomiast stos operandów korzysta tylko z `push` i `pop`,
ponieważ ewaluacja ONP nie wymaga podglądania szczytu ani sprawdzania pustości. Obie fazy
nigdy nie działają jednocześnie. Całość napisana jest w **paradygmacie strukturalnym**
(bez klas i obiektów), a logika podzielona jest na czytelne funkcje.

**Obsługiwane operatory:**

| Operator | Znaczenie    | Priorytet | Łączność     |
|:--------:|:-------------|:---------:|:-------------|
| `+`      | dodawanie    | 1         | lewostronna  |
| `-`      | odejmowanie  | 1         | lewostronna  |
| `*`      | mnożenie     | 2         | lewostronna  |
| `/`      | dzielenie    | 2         | lewostronna  |
| `^`      | potęgowanie  | 3         | prawostronna |

Program rozumie nawiasy okrągłe `( )`, liczby wielocyfrowe (np. `123`) oraz
zmiennoprzecinkowe (np. `3.14`), a błędne wejścia kończy czytelnym komunikatem zamiast
zawieszeniem się programu. Wykrywane są:

- puste wyrażenie,
- wyrażenie dłuższe niż `100` znaków,
- niedozwolony znak,
- niezbalansowane nawiasy,
- **niepoprawna składnia** - dwa operatory pod rząd, operator na początku/końcu,
  puste nawiasy czy źle zapisana liczba (walidacja prostym automatem stanów),
- dzielenie przez zero (zgłaszane jako błąd, bez zawieszenia programu).

**Ograniczenia:** wyrażenie wpisuje się **bez spacji**, bo program czyta je jednym
`cin >> rownanie` (spacja zakończyłaby wczytywanie). Nie jest obsługiwany minus unarny -
zapisy w stylu `-2` czy `2*-1` zostaną odrzucone przez walidację składni.

---

## <img src="https://img.icons8.com/color/48/source-code.png" alt="kompilacja" width="24" /> Sposób kompilacji

Wymagany kompilator C++ obsługujący standard **C++11 lub nowszy** (np. `g++` z pakietu GCC).

```bash
g++ -Wall -Wextra -std=c++17 -o aplikacja aplikacja.cpp
```

Program kompiluje się **bez żadnych ostrzeżeń** przy `-Wall -Wextra`.

W systemie Windows (np. MinGW) plik wynikowy to `aplikacja.exe`:

```bat
g++ -Wall -Wextra -std=c++17 aplikacja.cpp -o aplikacja.exe
```

---

## <img src="https://img.icons8.com/color/48/console.png" alt="uruchomienie" width="24" /> Uruchomienie i przykłady użycia

Po uruchomieniu program prosi o wpisanie wyrażenia (bez spacji), a następnie wypisuje
tablicę wag, postać ONP i wynik.

```bash
    .\aplikacja.exe
```

**Przykład 1** - kolejność działań i potęgowanie:

```text
Podaj rownanie: (2+3)*2-6^2
0 | 11 | 0 | 2 | 0 | 1 | 0 | 3 | 0 | 
Postac ONP: 2 3 + 2 * 6 2 ^ -
Wynik: -26
```

**Przykład 2** - nawiasy i liczby zmiennoprzecinkowe w wyniku:

```text
Podaj rownanie: 3+4*2/(1-5)^2
0 | 1 | 0 | 2 | 0 | 2 | 0 | 11 | 0 | 3 | 0 | 
Postac ONP: 3 4 2 * 1 5 - 2 ^ / +
Wynik: 3.5
```

**Przykład 3** - łączność prawostronna potęgowania (`2^(3^2) = 512`):

```text
Podaj rownanie: 2^3^2
0 | 3 | 0 | 3 | 0 | 
Postac ONP: 2 3 2 ^ ^
Wynik: 512
```

**Przykład 4** - zagnieżdżone nawiasy:

```text
Podaj rownanie: 2*(3+4*(5-1))
0 | 2 | 0 | 11 | 0 | 12 | 0 | 21 | 0 | 
Postac ONP: 2 3 4 5 1 - * + *
Wynik: 38
```

**Przykład 5** - wykrywanie dzielenia przez zero:

```text
Podaj rownanie: 2/0
0 | 2 | 0 | 
Postac ONP: 2 0 / 
Blad: dzielenie przez zero!
```

**Przykład 6** - liczby zmiennoprzecinkowe na wejściu:

```text
Podaj rownanie: 3.14*2+1
0 | 0 | 0 | 0 | 2 | 0 | 1 | 0 | 
Postac ONP: 3.14 2 * 1 +
Wynik: 7.28
```




| Wejście (notacja infiksowa) | Postać ONP | Wynik |
|---|---|---|
| `(2+3)*2-6^2` | `2 3 + 2 * 6 2 ^ -` | `-26` |
| `3+4*2/(1-5)^2` | `3 4 2 * 1 5 - 2 ^ / +` | `3.5` |
| `2^10` | `2 10 ^` | `1024` |
| `2^3^2` | `2 3 2 ^ ^` | `512` |
| `123+45` | `123 45 +` | `168` |

> **Jak czytać tablicę wag:** pierwsza linia to priorytet każdego znaku poza nawiasami.
> Liczby mają `0`, operatory - priorytet bazowy powiększony o `10` za każdy poziom zagnieżdżenia
> w nawiasach (stąd `+` wewnątrz nawiasu ma wagę `11`). To informacja pomocnicza; sama
> konwersja realizowana jest algorytmem Shunting-yard.