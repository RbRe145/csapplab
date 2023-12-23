# csim思路
> 根据s, E, b分配cache大小，在实际的代码中用malloc实现这个cache
> L trace代表从mem读取数据，S代表写入数据到mem，M代表先把数据修改？
> 在实际的计算机内存中，每一个block是最小的单元，由于实验中不涉及具体的数据，之需要模拟行为
> 因此，可以以line作为最小的单元，每个line中不需要有实际的数据
> 根据load, store, modify的顺序完善函数，先看完九哥的视频再行动