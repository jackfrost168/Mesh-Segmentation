A Feature Sensitive and Robust Mesh Segmentation Algorithm
==========================================================
Abstract
--------
#### It is very difficult to segment 3D meshes in computer graphics and computer vision. This paper proposes a novel mesh segmentation algorithm, which can obtain perception-aware results conveniently. First, we perform a rapid over-segmentation using adaptive space partition, and generate a series of superfacets. Second, we introduce a feature-sensitive region fusion algorithm to merge similar superfacets to obtain the segmentation result. Various experiments show that the proposed algorithm is able to handle 3D meshes with complex shape and rich details robustly and effectively. Even though the mesh models have noises, holes, missing parts, topology disturbances, pose changes or sampling differences, our algorithm can still achieve satisfactory segmentation results.

#### Key words: mesh segmentation; region fusion; feature sensitive; robustness

Process of Segmentation
-----------------------
![](https://github.com/JackFrost168/Mesh-Segmentation/raw/master/imgs/ProcessOfSegmentation.jpg)

Results
-------
### Results of Diffferent Model 
![](https://github.com/JackFrost168/Mesh-Segmentation/raw/master/imgs/differentclass.jpg)
### Results of Different Pose
![](https://github.com/JackFrost168/Mesh-Segmentation/raw/master/imgs/DifferentPose.jpg)
### Results with Noise
![](https://github.com/JackFrost168/Mesh-Segmentation/raw/master/imgs/WithNoise.jpg)
### Results with Missing Part
![](https://github.com/JackFrost168/Mesh-Segmentation/raw/master/imgs/WithMissingPart.jpg)
### Results with Holes
![](https://github.com/JackFrost168/Mesh-Segmentation/raw/master/imgs/WithHoles.jpg)
### Results of Different Sampling
![](https://github.com/JackFrost168/Mesh-Segmentation/raw/master/imgs/DifferentSampling.jpg)
