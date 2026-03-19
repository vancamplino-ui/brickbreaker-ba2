# Arbre de dépendance des fichiers

Ce document montre les dépendances entre les fichiers du projet à partir des `#include`.

---

## 1. Vue d'ensemble

```mermaid
flowchart LR

    MAIN[main.cc]

    GAMECC[game.cc]
    GAMEH[game.h]

    PADDLE[paddle.h et paddle.cc]
    BALL[ball.h et ball.cc]
    BRICK[brick.h et brick.cc]

    MESSAGE[message.h et message.cc]

    CONST[constants.h]
    TOOLS[tools.h et tools.cc]

    STD[bibliotheque standard]

    MAIN --> GAMEH
    GAMECC --> GAMEH
    GAMECC --> MESSAGE
    GAMECC --> CONST

    GAMEH --> PADDLE
    GAMEH --> BALL
    GAMEH --> BRICK
    GAMEH --> CONST

    PADDLE --> CONST
    PADDLE --> TOOLS
    BALL --> CONST
    BALL --> TOOLS
    BRICK --> CONST
    BRICK --> TOOLS

    CONST --> TOOLS

    MAIN --> STD
    GAMECC --> STD
    GAMEH --> STD
    MESSAGE --> STD
    TOOLS --> STD

    classDef entry fill:#1d4ed8,color:#ffffff,stroke:#1e3a8a,stroke-width:2px
    classDef coord fill:#dbeafe,color:#0f172a,stroke:#60a5fa,stroke-width:2px
    classDef model fill:#e0f2fe,color:#0f172a,stroke:#0284c7,stroke-width:2px
    classDef tools fill:#dcfce7,color:#14532d,stroke:#16a34a,stroke-width:2px
    classDef msg fill:#fee2e2,color:#7f1d1d,stroke:#ef4444,stroke-width:2px
    classDef std fill:#f3f4f6,color:#111827,stroke:#9ca3af,stroke-width:2px

    class MAIN entry
    class GAMECC,GAMEH coord
    class PADDLE,BALL,BRICK model
    class CONST,TOOLS tools
    class MESSAGE msg
    class STD std
```

---

## 2. Zoom sur game

```mermaid
flowchart TD

    GAMECC[game.cc]
    GAMEH[game.h]
    PAD[paddle.h]
    BAL[ball.h]
    BRI[brick.h]
    MSG[message.h]
    CON[constants.h]
    TOO[tools.h]

    GAMECC --> GAMEH
    GAMECC --> MSG
    GAMECC --> CON

    GAMEH --> PAD
    GAMEH --> BAL
    GAMEH --> BRI
    GAMEH --> CON

    PAD --> TOO
    PAD --> CON
    BAL --> TOO
    BAL --> CON
    BRI --> TOO
    BRI --> CON
    CON --> TOO

    classDef root fill:#1e3a8a,color:#ffffff,stroke:#172554,stroke-width:3px
    classDef head fill:#bfdbfe,color:#111827,stroke:#60a5fa,stroke-width:2px
    classDef dep fill:#d1fae5,color:#111827,stroke:#22c55e,stroke-width:2px
    classDef extra fill:#fee2e2,color:#111827,stroke:#ef4444,stroke-width:2px

    class GAMECC root
    class GAMEH head
    class PAD,BAL,BRI,CON,TOO dep
    class MSG extra
```

---

## 3. Lecture simple

- `main.cc` lance le programme et inclut `game.h`.
- `game.cc` dépend de `game.h`, `message.h` et `constants.h`.
- `game.h` relie `paddle.h`, `ball.h` et `brick.h`.
- `paddle.h`, `ball.h` et `brick.h` utilisent `tools.h` et `constants.h`.
- `constants.h` dépend de `tools.h`.
- `message.cc` est séparé du modèle et sert aux messages d'erreur.

---

## 4. Affichage dans VS Code

- ouvrir ce fichier Markdown
- utiliser `Cmd + Shift + V`
- ou clic droit puis `Open Preview`
