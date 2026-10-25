# VulkanTuto

Projet de rendu graphique réalisé en **C++ avec Vulkan**.

## Présentation

Ce projet a pour objectif de découvrir l'API graphique Vulkan et de développer une application capable d'afficher des objets 3D texturés.
Il utilise **Vulkan-Hpp RAII** pour gérer les ressources Vulkan et repose sur une architecture en plusieurs classes C++.
Le projet comprend deux programmes utilisant les mêmes composants de rendu.

## Fonctionnalités principales

- Initialisation de Vulkan et création d'une fenêtre avec GLFW.
- Affichage d'objets 3D à l'aide de vertex buffers et index buffers.
- Application de textures.
- Utilisation des matrices Model, View et Projection.
- Gestion de la profondeur avec un depth buffer.
- Gestion du redimensionnement de la fenêtre.
- Utilisation des validation layers pour le débogage.

## Technologies utilisées

| Technologie | Utilisation |
|---|---|
| C++23 | Développement de l'application |
| Vulkan | API graphique |
| Vulkan-Hpp RAII | Gestion des ressources Vulkan |
| GLFW | Gestion de la fenêtre |
| GLM | Calculs mathématiques et transformations 3D |
| stb_image | Chargement des textures |
| Slang / SPIR-V | Shaders |
| Visual Studio 2026 | Environnement de développement |

## Prérequis

- Windows 10 ou Windows 11.
- Visual Studio 2026 avec les outils de développement C++.
- Vulkan SDK installé.
- Carte graphique compatible Vulkan.
- Bibliothèques et chemins d'inclusion correctement configurés dans Visual Studio.

## Installation

### 1. Cloner le dépôt

```bash
git clone https://github.com/hbelaskri/VulkanTuto.git
```

### 2. Ouvrir le projet

Ouvrir le fichier `VulkanTuto.slnx` dans Visual Studio 2026.

### 3. Configurer la compilation

- Sélectionner la plateforme **x64**.
- Vérifier les chemins du Vulkan SDK et des bibliothèques.
- Sélectionner le programme à compiler.

## Exécution des deux programmes

Le projet possède deux fichiers contenant une fonction `main()` :

- `VulkanTuto/VulkanTuto.cpp`
- `VulkanTuto/MainClient.cpp.cpp`

Les deux programmes partagent les mêmes classes de rendu.

Pour exécuter l'un des programmes dans Visual Studio :

1. Faire un clic droit sur le fichier contenant le `main()` à désactiver.
2. Sélectionner **Propriétés**.
3. Dans **Général**, définir **Exclu de la génération** sur **Oui**.
4. Vérifier que le fichier du programme souhaité est inclus dans la génération.
5. Générer la solution.

Un seul fichier contenant `main()` doit être compilé à la fois.
Le resultat sera different selon le main uitilise.

## Ressources

Les fichiers nécessaires :

- `Textures/` : images utilisées pour les textures.
- `shaders/` : shaders Slang, fichiers SPIR-V et script de compilation.
- `include/` : en-têtes de bibliothèques.
- `libs/` : bibliothèques GLFW.

## Limites connues

- **Configuration :** la compilation nécessite une installation du Vulkan SDK et une configuration correcte des bibliothèques.
- **Exécution :** les shaders et les textures doivent être accessibles depuis le répertoire de travail du programme.
- **Programmes :** les deux points d'entrée `main()` ne peuvent pas être compilés simultanément. Le programme souhaité doit être sélectionné dans Visual Studio.


## Références utilises

- [Tutoriel officiel Vulkan — Khronos](https://docs.vulkan.org/tutorial/latest/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)
- [stb_image](https://github.com/nothings/stb)

