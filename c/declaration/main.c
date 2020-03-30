// c 中声明和定义
int i1 = 1;     // 定义，外部链接
// 试探性定义，表现为声明，因为 i1 已定义
int i1;
// 声明，引用前面的定义
extern int i1; 

int main() {
    return 0;
}