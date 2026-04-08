# Module gui

Ce dossier contiendra le code lie a l'interface GTKmm.

Role prevu pour le rendu 2:
- creer et gerer la fenetre GTKmm;
- contenir la zone de dessin;
- gerer les boutons;
- recevoir les evenements clavier;
- recevoir les evenements souris;
- declencher le rafraichissement de l'affichage;
- faire le lien entre `model` et `graphic`.

Ce module ne doit pas porter la logique physique du jeu: cette responsabilite reste dans `model`.
`Game` reste la source de verite, et l'interface doit lire son etat sans le dupliquer.
