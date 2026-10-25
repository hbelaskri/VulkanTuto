
# Checklist de recette — VulkanTuto

## Tests

| Test | Résultat attendu | Statut |
|---|---|---|
| Clonage du dépôt | Le projet compile dans un autre dossier. | oui |
| Lancement | L'objet 3D texturé s'affiche. | oui |
| Transformation | L'objet peut être transformé depuis l'application. | oui |
| Profondeur | Les surfaces se cachent correctement, même en inversant leur ordre de dessin. |oui |
| Redimensionnement | Le rendu s'adapte à la fenêtre. | oui |
| Réduction / restauration | Le rendu reprend sans problème. | oui |
| Texture absente | Un message d'erreur apparaît. | oui |
| Fermeture | Le programme se ferme sans erreur Vulkan. |oui |
| Deuxième client | Le renderer fonctionne avec un autre programme. | oui |

## Limites rencontrées

Le projet a été développé sous Windows avec Visual Studio 2026.
Il nécessite le Vulkan SDK.

Pour lancer les deux programmes, je dois sélectionner le fichier
main() à compiler dans Visual Studio.
Il reste à terminer les vérifications de la checklist,
notamment les tests de robustesse et les validation layers.

## Bilan

J'ai réalisé un renderer 3D avec Vulkan en séparant le code
en plusieurs classes.

Le projet comprend deux programmes clients permettant de
réutiliser les mêmes composants de rendu.
Le resultat sera soit un cube 3D soit un triangle 2d.


