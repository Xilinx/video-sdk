
There has been a longstanding goal for video engineers to quantitatively determine the output quality of an encoder without having to watch and inspect every individual frame. This has led to an evolution of algorithmic solutions, the most common of which are:

- `Peak Signal to Noise Ratio (PSNR) <https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio>`__

- `Structural Similarity Index Metric (SSIM) <https://en.wikipedia.org/wiki/Structural_similarity>`__

- `Video Multimethod Assessment Fusion (VMAF) <https://en.wikipedia.org/wiki/Video_Multimethod_Assessment_Fusion>`__

- `Mean Opinion Score (MOS) - Humans visually watching the screen and providing feedback <https://en.wikipedia.org/wiki/Mean_opinion_score>`__

Many people will argue which metric is best (although PSNR is commonly considered the least accurate). Jan Ozer from the Streaming Media Center posted his experimental correlation of MOS vs the above metrics. You can review the findings `here. <https://streaminglearningcenter.com/wp-content/uploads/2017/08/PSRN-vs.-VMAF-vs.-SSIMPlus.pdf>`__

Furthermore, due to the industry standard of tracking encoder "performance" to quantitative metrics like the ones listed above, many encoders have "taught to the test"; that is, they provide different command-line arguments that will give higher scores but may look worse to the human eye. For example, common CPU encoders `x264 <https://code.videolan.org/videolan/x264>`__ and `x265 <http://hg.videolan.org/x265>`__ have a ``tune`` parameter which optimizes to objective metrics.

This page discusses the |SDK| command line flags used to optimize for objective quality (scores) and subjective quality (visual appeal) and provides additional details as to what is happening behind the scenes and why.