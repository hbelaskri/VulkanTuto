# Architecture du projet VulkanTuto

## 1. Présentation

Pour ce projet, j'ai développé un renderer 3D en C++ avec Vulkan en suivant le tutoriel officiel Khronos.

Au début, le but était surtout de comprendre comment fonctionne Vulkan. Ensuite, j'ai séparé mon code en plusieurs classes pour éviter de tout mettre dans un seul fichier et pour pouvoir réutiliser mon renderer dans un autre projet.

J'ai également créé deux programmes qui utilisent les mêmes classes de rendu, mais avec des données différentes.

## 2. Organisation du projet

           VulkanTuto.cpp       MainClient.cpp.cpp
                 |                     |
                 +----------+----------+
                            |
                            v
                       Application
                            |
         +------------------+------------------+
         |                  |                  |
         v                  v                  v
      Renderer           SwapChain          Pipeline
         |                  |                  |
         +------------------+------------------+
                            |
          +-----------------+------------------+
          |                 |                  |
          v                 v                  v
     VertexBuffer        Texture         UniformBuffer
          |                                    |
          v                                    |
       Vertex.h                                |
          |                                    |
          +-----------------+------------------+
                            |
                            v
                       CommandBuffer

       Debug : aide au débogage de Vulkan
Ce schéma montre surtout comment j'ai organisé les différentes parties du projet. En réalité, certaines classes utilisent plusieurs autres classes.

La classe `Application` sert à lancer et à gérer l'application, tandis que les autres classes s'occupent des différentes étapes du rendu Vulkan.

## 3. Rôle des classes

J'ai essayé de donner un rôle précis à chaque classe pour que mon code soit plus facile à comprendre.

| Classe ou fichier | Rôle dans le projet |
|---|---|
| `VulkanTuto.cpp` | Contient le `main()`. |
| `MainClient.cpp.cpp` | Contient le `main()` du deuxième programme client, utilisé pour tester la réutilisation du renderer. |
| `Application` | Gère la fenêtre, initialise les composants et exécute la boucle principale. |
| `Renderer` | Initialise Vulkan, sélectionne le GPU et fournit les ressources Vulkan communes. |
| `SwapChain` | Gère les images de présentation, la profondeur et le redimensionnement. |
| `Pipeline` | Configure le pipeline graphique et les shaders. |
| `CommandBuffer` | Enregistre les commandes, les envoie au GPU et gère la synchronisation. |
| `VertexBuffer` | Crée et gère les buffers contenant les sommets et les indices. |
| `UniformBuffer` | Gère les matrices Model, View et Projection ainsi que les descriptor sets. |
| `Texture` | Charge les textures et prépare les ressources nécessaires à leur utilisation avec Vulkan. |
| `Vertex.h` | Définit la structure des sommets utilisée pour représenter la géométrie. |
| `Debug` | Regroupe les éléments liés au débogage du projet. |

## 4. Interface publique et réutilisation

Un des objectifs du projet était de pouvoir utiliser le renderer avec plusieurs applications.

Pour cela, j'ai fait en sorte de pouvoir donner à `Application` les sommets, les indices et le chemin de la texture directement dans son constructeur.
Ensuite, j'utilise les méthodes `Init()` et `Run()` pour initialiser et lancer le programme.
J'ai aussi ajouté une méthode dans `UniformBuffer` pour pouvoir modifier la matrice du modèle :
Cela permet de changer la transformation de l'objet sans devoir modifier directement le fonctionnement du rendu.
Pour tester cette réutilisation, j'ai créé un deuxième programme client avec des données différentes, mais qui utilise les mêmes classes.

## 5. Gestion des ressources

Pour gérer les ressources Vulkan, j'utilise **Vulkan-Hpp RAII**.
Le principe est que les objets C++ s'occupent de libérer les ressources Vulkan qu'ils possèdent lorsqu'ils sont détruits.
Par exemple, `Renderer` possède les ressources principales comme l'instance Vulkan et le périphérique logique. Les autres classes possèdent les ressources qui correspondent à leur rôle, comme les buffers, les textures ou le pipeline.
Certaines classes gardent aussi des références vers `Renderer` ou vers d'autres composants dont elles ont besoin.
Il faut quand même faire attention à l'ordre de destruction des objets, car certaines ressources dépendent d'autres ressources. Il faut également attendre que le GPU ait terminé d'utiliser une ressource avant de la détruire.

## 6. Fonctionnement du rendu

Pour afficher une image, le programme suit plusieurs étapes :

1. Il attend que les ressources de la frame soient disponibles.
2. Il récupère une image de la swapchain.
3. Il met à jour les matrices utilisées pour afficher l'objet.
4. Il enregistre les commandes de rendu.
5. Il envoie ces commandes au GPU.
6. Il présente l'image dans la fenêtre.

Ces opérations sont principalement gérées dans la classe `CommandBuffer`, avec les fonctions `drawFrame()` et `recordCommandBuffer()`.

## 7. Transformations et profondeur

Pour positionner les objets en 3D, j'utilise trois matrices :

- **Model** : pour la position, la rotation et la taille de l'objet.
- **View** : pour représenter la caméra.
- **Projection** : pour afficher la scène 3D avec une perspective.

Ces matrices sont calculées avec GLM et envoyées aux shaders grâce aux uniform buffers.

J'ai aussi intégré un **depth buffer** pour gérer la profondeur. Cela permet d'afficher correctement les surfaces qui se trouvent devant ou derrière d'autres surfaces, même si elles se superposent à l'écran.

## 8. Redimensionnement de la fenêtre

J'ai prévu la gestion du redimensionnement dans la classe `SwapChain`.
Lorsque la taille de la fenêtre change, certaines ressources Vulkan doivent être recréées pour correspondre aux nouvelles dimensions.
La matrice de projection utilise également les dimensions de la swapchain pour garder les bonnes proportions.
Cela permet d'adapter le rendu lorsque la fenêtre change de taille.

## 9. Intégration dans le futur Moteur 3D

Le but de cette architecture est aussi de préparer la réutilisation du renderer dans notre futur projet Moteur 3D.
Le fait d'avoir séparé les différentes responsabilités et de pouvoir fournir la géométrie et la texture depuis un programme client est une première étape.

## 10. Conclusion

Ce projet m'a permis de mieux comprendre le fonctionnement de Vulkan, notamment l'initialisation, les buffers, les textures, les commandes de rendu et la synchronisation.
J'ai aussi appris à organiser un projet C++ en séparant les responsabilités dans plusieurs classes.
Même si l'architecture peut encore être améliorée, elle me permet déjà de réutiliser les mêmes composants de rendu dans deux programmes différents et constitue une première base pour le futur Moteur 3D.
