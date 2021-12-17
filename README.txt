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

 En jeu :
  W : Avancer
  S : Reculer
  A/D : Roulis
  Shift/Espace : Monter/Descendre
  Echap : Pause
  F2 : Toggle 1ere/3e personne. 3e personne par
   défaut.

 Souris : Orientation de la caméra (détermine la
  direction vers laquelle on avance/recule)

 Menu de démarrage:
  1/2 Choix de la difficulte

 Menu Pause:
  Echap : Reprendre la partie
   0/9: Changer de shader (cyclique)

 Fin de jeu:
  R relancer la partie.

====================================================
			Notes
====================================================

 Le jeu ne se lance qu'en DEBUG.

 Le premier lancement peut prendre un certain temps,
 mais si c'est le cas, les suivants seront plus
 courts.
 
 Pour mettre pause il faut appuyer sur Echap. On 
 reprend alors le contrôle de la souris et on peut
 fermer le jeu normalement.
 
 La structure est finalisée et tente de s'approcher 
 au maximum de ce qui a été proposé au volet 1. 
 L'on reprend les concepts de Texture et GameManager. 
 L'on a aussi une partie de la suite
 d'héritage partant de la classe entité, qui est ici
 CObjetMesh, qui devient MovingObject, qui devient
 Player, Planet, Asteroid, Monster et NPC. La classe 
 Portal et TunnelComponent représentent des objets 
 statiques et héritent également de la classe CObjetMesh.
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
 flickering lors du changement. Nous pouvons voir
 clairement les effets de la tessellation notamment
 au niveau des asteroides géants présents dans la
 zone rouge.

====================================================
		    Effets HLSL
====================================================

 Tout d'abord nous avons le LOD mentionné plus haut
 mais nous avons également de l'alpha blending au 
 niveau de la height map dans notre tunnel. Nous 
 avons aussi mis en place plusieurs autres effets 
 tels que le blur lors de la téléportation entre les
 zones, différents types de shaders pour les 
 différentes formes de  daltonisme, ainsi qu'un 
 shader en niveux de gris  pour donner un visuel 
 plus retro au jeu.
 
====================================================
		     Bugs Connus
====================================================

 Malgré tous nos efforts, quelques bugs se sont
 glissés dans notre projet. Notamment, il se peut
 qu'au lancement le menu ne s'affiche pas. Il est
 pourtant bien présent et actif, il suffit d'appuyer
 sur 1 ou 2 pour lancer en facile/difficile. Cette
 situation est visible avec un ecran bleu-noir au
 demarrage. Ce bug semble généralement avoir lieu
 lors du premier lancement après compilation.
