# Lighthouse in the sea

58/2020 - Jana Radivojević \
Scene of a boat passing by a lone lighthouse in the sea

## Controls

W -> Camera forwards\
S -> Camera backwards\
A -> Camera left\
D -> Camera right\
G -> Activate light event chain\
K -> Toggle GUI


## Features
When the key G is pressed, lighthouse light will start rotating, after a few seconds the boat starts moving and the lightouse light turns red.\
When the key K is pressed, you can toggle GUI, in which you can change the settings of the directional and spot light.
### Fundamental:

[X] Model with lighting
[X] Two types of lighting with customizable colors and movement through GUI or ACTIONS
[X] {PRESS G} --- AFTER 2 SECONDS---Triggers---> {SPOTLIGHT ROTATING} ---> AFTER 4 SECONDS---Triggers---> {SPOTLIGHT COLOR TURNING TO RED AND THE BOAT STARTS MOVING}

### Group A:

[ ] Frame-buffers with post-processing   
[ ] Instancing  
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping

### Group B:
[ ] Bloom with the use of HDR  
[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO