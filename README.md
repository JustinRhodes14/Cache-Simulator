# Cache-Simulator

This project was completed in my Computer Architecture class. The goal of this project was to simulate three different configurations of caches: direct-mapped caches, set-associative caches, and fully-associative caches. Furthermore, we also had to implement two replacement policies: least recently used policy (LRU), as well as first in first out policy (FIFO). The cache simulated in this program was also a one level write through cache, and could simulate caches with and without prefetching.

This project was one of my favorite projects that I have done so far in my computer science career, despite all the sleepless nights of debugging and confusion. The reason why this project sticks out so much to me is the fact that I truly felt like I was able to understand how caches worked after completing this project. It truly challenged me to dive deeper and really understand how caches work. Constantly looking back at my notes and drawing out my thoughts and ideas on whiteboards, eventually translating my thoughts to diagrams of caches is what really allowed for me to complete this project.

While the program works and is able to accuratley simulate all the configurations of caches, it can still be improved. I extracted the tag, index, and block offset bits using character arrays, however, this proved to be a slight hinderance on the run time of the program. Eventually, I wish to implement bitwise operations to extract these things to improve the performance of the program.

Never the less, I really enjoyed this project and truly learned a lot from it, and I am very proud of the final result for this project.
