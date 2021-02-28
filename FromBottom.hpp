using namespace std;
// ボトムアップ型の自動微分
class Node {
    public:
        long double val;
        long double dval;
        Node(long double xval = 0, long double dxval = 0) {
            val = xval;
            dval = dxval;
        }

        void select_as_delta() {
            dval = 1.0;
        }

        // 四則演算
        friend const Node operator + (Node rhs, Node lhs) {
            return Node(rhs.val + lhs.val, rhs.dval + lhs.dval);
        }

        friend const Node operator - (Node rhs, Node lhs) {
            return Node(rhs.val - lhs.val, rhs.dval - lhs.dval);
        }

        friend const Node operator * (Node rhs, Node lhs) {
            return Node(rhs.val * lhs.val, rhs.dval * lhs.val + rhs.val * lhs.dval);
        }

        friend const Node operator / (Node hx, Node gx) {
            return Node(hx.val / gx.val, (hx.dval * gx.val - hx.val * gx.dval) / (gx.val  * gx.val));
        }
        // 一般的な関数
        // *などとの処理順番の問題があるため、powは^にしない
        friend const Node pow (Node rhs, Node lhs) {
            return Node(pow(rhs.val, lhs.val), pow(rhs.val, lhs.val) * lhs.dval * log(rhs.val) + pow(rhs.val, lhs.val - 1) * lhs.val * rhs.dval);
        }

        friend const Node sin(Node child) {
            return Node(sin(child.val), child.dval * cos(child.val));
        }

        friend const Node cos(Node child) {
            return Node(cos(child.val), child.dval * -1 * sin(child.val));
        }

        friend const Node tan(Node child) {
            return Node(tan(child.val), (child.dval / cos(child.val)) / cos(child.val));
        }

        friend const Node exp(Node child) {
            long double child_exp = exp(child.val);
            return Node(child_exp, child.dval * child_exp);
        }

        friend const Node log(Node child) {
            return Node(log(child.val), child.dval / child.val);
        }

        friend const Node log(long double base, Node child) {
            return Node(log(child.val) / log(base), child.dval /(child.val * log(base)));
        }

        friend Node sqrt(Node child) {
            return Node(sqrt(child.val), child.dval / (sqrt(child.val) * 2));
        }
};