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
 sera améliorée pour le volet 3, mais tente de
 s'approcher au maximum de ce qui a été proposé au
 volet 1. L'on reprend les concepts de Texture et
 GameManager. L'on a aussi une partie de la suite
 d'héritage partant de la classe entité, qui est ici
 CObjetMesh, qui devient MovingObject, qui devient
 Player, Planet, Asteroid et NPC.
 Cependant, il y a eu quelques changements.
 
 - Afin d'éviter la redondance des classes CCamera,
 les trois caméras prévues (FreeCam, 3e personne et
 1ere personne) ont été fusionnées en une camera
 avec des pointeurs vers différentes fonctions
 update(), qui font différentes mises à jour de la
 position de la caméra.
 
====================================================
	Description du format des données et 
	      des outils de lecture
====================================================

 Nous avons utilisé le chargeur d'objet fourni dans
 le cours. Il y a donc, et ce pour chaque fichier
 chargé, le chargement de l'objet, d'un fichier obm
 pré-généré. Dans un autre programme, à l'avance,
 l'on vient générer ces .obm à partir de .obj. 
 
====================================================
			LOD
====================================================

Pour ce qui est du LOD nous avons mis en place la
tesselation dans le shader Miniphong.fx. Nous avons
également un shader nommé MiniphongClassic.fx qui
est le shader de base.
Lors de l'animation de chaque objet on teste la
distance par rapport a la caméra et si l'on est
assez proche on active alors la tesselation pour
ajouter du détail. Nous pouvons voir un leger 
flickering lors du changement. Pour regler ceci,
nous allons augmenter la distance d'activation de
la tessellation. Pour le volet 2 cette distance 
est assez courte pour le voir facilement. 
