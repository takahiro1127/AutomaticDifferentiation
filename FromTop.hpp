#include <map>
#include <vector>
using namespace std;
// トップダウン型の自動微分
class Node {
    public:
        // 自分とfとの微分
        long double dfdg;
        // 自身の最終的な出力値
        long double output;
        int flag;
        int usedMaxFlag;
        // このNodeのinputとなったNodeと、dこのNode/d係数とした微分の値のペア
        // flagを参照したsearch機能を持つ、管理クラスを作るのもあり。
        vector<Node> inputs;
        vector<long double> differentials;
        map<int, long double> flagToDiff;

        Node(long double val = 1) {
            usedMaxFlag = 0;
            flag = 0;
            output = val;
            dfdg = 1;
        }

        void setDifferental() {
            flagToDiff = getDifferental();
        }

        map<int, long double> getDifferental(long double diff_from_top = 1) {
            dfdg = diff_from_top;
            //再起関数の終了条件
            //また、ここで初めて、flagToDiffに値がセットされる。
            if (flag != 0) {
                flagToDiff.insert(make_pair(flag, dfdg));
            }
            vector<Node>::iterator NodesIte = inputs.begin();
            vector<long double>::iterator DiffsIte = differentials.begin();
            for (; NodesIte != inputs.end() && DiffsIte != differentials.end(); NodesIte++, DiffsIte++) {
                mergeFlagToDiff(NodesIte->getDifferental(*DiffsIte * diff_from_top));
            }
            return flagToDiff;
        }

        void mergeFlagToDiff(map<int, long double> returnedFlagToDiff) {
            map<int, long double>::iterator flagAndDiff = returnedFlagToDiff.begin();
            for (; flagAndDiff != returnedFlagToDiff.end(); flagAndDiff++) {
                if (flagToDiff.find(flagAndDiff->first) == flagToDiff.end()) {
                    flagToDiff.insert(make_pair(flagAndDiff->first, flagAndDiff->second));
                } else {
                    flagToDiff[flagAndDiff->first] += flagAndDiff->second;
                }
            }
        }

        long double getDiff(Node node) {
            return flagToDiff[node.flag];
        }

        void setDiff(Node& node) {
            node.dfdg = getDiff(node);
        }

        //managerクラスなどで管理するのはあり。
        void setFlagAsVariable(int flag_number) {
            flag = flag_number;
            usedMaxFlag = max(flag_number, usedMaxFlag);
        }

        void setAsInput(Node input,long double dNoodedinput) {
            usedMaxFlag = max(usedMaxFlag, input.usedMaxFlag);
            inputs.push_back(input);
            differentials.push_back(dNoodedinput);
        }

        // 基本演算
        friend Node operator + (Node left, Node right) {
            Node result = Node(left.output + right.output);
            result.setAsInput(left, 1);
            result.setAsInput(right, 1);
            return result;
        }

        friend Node operator - (Node left, Node right) {
            Node result = Node(left.output - right.output);
            result.setAsInput(left, 1);
            result.setAsInput(right, -1);
            return result;
        }

        friend Node operator * (Node left, Node right) {
            Node result = Node(left.output * right.output);
            result.setAsInput(left, right.output);
            result.setAsInput(right, left.output);
            return result;
        }

        friend Node operator / (Node left, Node right) {
            Node result = Node(left.output / right.output);
            result.setAsInput(left, 1 / right.output);
            result.setAsInput(right, -1 * left.output / (right.output * right.output));
            return result;
        }

        // 一般的な関数
        friend Node exp(Node index) {
            Node result = Node(exp(index.output));
            result.setAsInput(index, result.output);
            return result;
        }

        friend Node sin(Node x) {
            Node result = Node(sin(x.output));
            result.setAsInput(x, cos(x.output));
            return result;
        }

        friend Node cos(Node x) {
            Node result = Node(cos(x.output));
            result.setAsInput(x, -1 * sin(x.output));
            return result;
        }

        friend Node tan(Node x) {
            Node result = Node(tan(x.output));
            result.setAsInput(x, 1 / (cos(x.output) * cos(x.output)));
            return result;
        }

        friend Node log(Node x) {
            Node result = Node(log(x.output));
            result.setAsInput(x, 1 / x.output);
            return result;
        }

        friend Node log(long double base, Node x) {
            Node result = Node(log(x.output) / log(base));
            result.setAsInput(x, 1 / (x.output * log(base)));
            return result;
        }

        friend Node pow (Node x, Node y) {
            Node result = Node(pow(x.output, y.output));
            result.setAsInput(x, y.output * pow(x.output, y.output - 1));
            result.setAsInput(y, result.output * log(x.output));
            return result;
        }

        friend Node sqrt(Node x) {
            Node result = Node(sqrt(x.output));
            result.setAsInput(x, 1 / (2 * result.output));
            return result;
        }
};