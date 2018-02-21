#define _CRT_SECURE_NO_WARNINGS

#include "ShapeContextCalculator.h"
#include <iostream>

int main()
{
    Mat frame1 = imread("1.bmp", CV_8UC1);
    Mat frame2 = imread("3.bmp", CV_8UC1);

    ShapeContextCalculator scc = ShapeContextCalculator(frame1, frame2, 1);

    //for (int i = 0; i < scc.sample[1].size(); i++)
    //{
    //    printf("sample[1][%d]: (%d, %d)\n", i, scc.sample[1][i].x, scc.sample[1][i].y);
    //}

    cout << scc.velocity(861, 622) << endl;
    
    //scc.showDebugAssignmentResult();
    //scc.writeDebugAssignmentGraph("debugAssignmentGraph.bmp");

    //scc.writeDebugSamplingPointGraph(0, "debugSamplingPointGraph0.bmp");
    //scc.writeDebugSamplingPointGraph(1, "debugSamplingPointGraph1.bmp");

    //scc.writeDebugShapeContextGraph(0, 0, "debugShapeContextGraph0.bmp");
    //scc.writeDebugShapeContextGraph(1, 0, "debugShapeContextGraph1.bmp");

    printf("all finished\n");
    system("pause");
    return 0;
}

//sample[1][0]: (1813, 1)
//sample[1][1]: (1811, 43)
//sample[1][2]: (1807, 85)
//sample[1][3]: (1803, 126)
//sample[1][4]: (1799, 167)
//sample[1][5]: (1799, 210)
//sample[1][6]: (1792, 251)
//sample[1][7]: (1775, 290)
//sample[1][8]: (1748, 317)
//sample[1][9]: (1709, 325)
//sample[1][10]: (1668, 307)
//sample[1][11]: (1625, 284)
//sample[1][12]: (1582, 267)
//sample[1][13]: (1540, 245)
//sample[1][14]: (1498, 217)
//sample[1][15]: (1456, 199)
//sample[1][16]: (1413, 193)
//sample[1][17]: (1370, 189)
//sample[1][18]: (1327, 187)
//sample[1][19]: (1284, 183)
//sample[1][20]: (1241, 179)
//sample[1][21]: (1198, 179)
//sample[1][22]: (1155, 181)
//sample[1][23]: (1112, 184)
//sample[1][24]: (1071, 197)
//sample[1][25]: (1029, 226)
//sample[1][26]: (991, 260)
//sample[1][27]: (954, 300)
//sample[1][28]: (923, 343)
//sample[1][29]: (900, 386)
//sample[1][30]: (884, 429)
//sample[1][31]: (869, 472)
//sample[1][32]: (866, 515)
//sample[1][33]: (861, 558)
//sample[1][34]: (861, 601)
//sample[1][35]: (861, 644)
//sample[1][36]: (881, 687)
//sample[1][37]: (908, 726)
//sample[1][38]: (935, 761)
//sample[1][39]: (960, 794)
//sample[1][40]: (1003, 819)
//sample[1][41]: (1046, 833)
//sample[1][42]: (1089, 837)
//sample[1][43]: (1132, 841)
//sample[1][44]: (1175, 841)
//sample[1][45]: (1218, 839)
//sample[1][46]: (1261, 833)
//sample[1][47]: (1304, 831)
//sample[1][48]: (1346, 829)
//sample[1][49]: (1389, 821)
//sample[1][50]: (1431, 813)
//sample[1][51]: (1472, 800)
//sample[1][52]: (1514, 785)
//sample[1][53]: (1557, 767)
//sample[1][54]: (1597, 748)
//sample[1][55]: (1632, 726)
//sample[1][56]: (1673, 703)
//sample[1][57]: (1713, 685)
//sample[1][58]: (1755, 681)
//sample[1][59]: (1785, 702)
//sample[1][60]: (1787, 743)
//sample[1][61]: (1787, 786)
//sample[1][62]: (1787, 829)
//sample[1][63]: (1787, 872)
//sample[1][64]: (1789, 915)
//sample[1][65]: (1794, 958)
//sample[1][66]: (1799, 1001)
//sample[1][67]: (1806, 1044)
//sample[1][68]: (1813, 1079)
//sample[1][69]: (1813, 1079)
//sample[1][70]: (1813, 1079)