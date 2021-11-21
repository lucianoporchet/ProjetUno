#pragma once

namespace PM3D
{
    //
	//   TEMPLATE : CSingleton
	//
	//   BUT : Template servant à construire un objet unique
	//
	//   COMMENTAIRES :
	//
	//        Comme plusieurs de nos objets représenteront des éléments uniques 
	//		  du système (ex: le moteur lui-même, le lien vers 
	//        le dispositif Direct3D), l'utilisation d'un singleton 
	//        nous simplifiera plusieurs aspects.
	//

	template <class T> class CSingleton
	{
	public:
        // Renvoie l'instance unique de la classe
        static T& GetInstance()
        {
            return Instance;
        }

	protected :

        // Constructeur par défaut
		CSingleton(void){}
	
        // Destructeur
		~CSingleton(void){}

	private :

        // Données membres
        static T Instance; // Instance de la classe

		// Déclarer les opérations de copie sans les
		// définir, pour empêcher leur utilisation
        CSingleton(CSingleton&);
        void operator =(CSingleton&);
  
	};

    // Instanciation du singleton
    template <class T> T CSingleton<T>::Instance ;
}