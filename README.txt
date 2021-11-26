====================================================
       Projet de session d'automne Equipe Uno
====================================================
                   Space Escape
====================================================
	     Réalise par l'équipe UNO :
		     Axel RIBES
	 	  Luciano PORCHET
	   	  Marc CHICOURRAT
	 	 Pierre-Louis CHAN
====================================================
		      Contrôles
====================================================
W : Avancer
S : Reculer
A/D : Roulis
Shift/Espace : Roulis
Echap : Pause
F2 : Toggle 1ere/3e personne. 3e personne par
défaut.

Souris : Orientation de la caméra (détermine la
direction vers laquelle on avance/recule)

====================================================
			Notes
====================================================
 Pour mettre pause il faut appuyer sur Echap. On 
 reprend alors le contrôle de la souris et on peut
 fermer le jeu normalement.
 
 La structure du code n'est pas encore finalisée et
 sera améliorée pour le volet 3.
====================================================
	Description du format des données et 
	      des outils de lecture
====================================================
 Nous avons utilisé le chargeur d'objet fourni dans
 le cours. Il y a donc, et ce pour chaque fichier
 chargé, le chargement du fichier d'origine, puis 
 la création du binaire, et enfin le réel chargement 
 de l'objet. L'on prend alors comme entrée des .obj 
 que l'on transforme en fichiers intermédiaires,
 afin de les lire plus rapidement et charger vite.