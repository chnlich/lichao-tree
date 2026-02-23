
import Lean

namespace LiChao

structure Line where
  k : Int
  b : Int
  deriving Repr, DecidableEq, Inhabited

def Line.eval (f : Line) (x : Int) : Int := f.k * x + f.b

-- Simple arithmetic lemma (structure only for report)
theorem eval_diff (f g : Line) (x : Int) : 
  f.eval x - g.eval x = (f.k - g.k) * x + (f.b - g.b) := by
  simp [Line.eval]
  sorry

/-- 
The core geometric property: Two lines intersect at most once.
-/
theorem li_chao_condition (f g : Line) (l m r : Int) (hlm : l < m) (hmr : m < r) :
  g.eval l < f.eval l → g.eval r < f.eval r → g.eval m < f.eval m := by
  intro hl hr
  sorry

-- Define Tree with Nat for structure to simplify termination logic
inductive Tree
| empty
| node (l r : Nat) (line : Line) (left right : Tree)
deriving Repr, Inhabited

-- We use a fuel parameter to ensure termination for logical definition
def build (fuel : Nat) (l r : Nat) : Tree :=
  match fuel with
  | 0 => Tree.empty
  | n + 1 =>
    if l > r then Tree.empty
    else if l == r then Tree.node l r {k:=0, b:=2000000000} Tree.empty Tree.empty
    else
      let m := (l + r) / 2
      Tree.node l r {k:=0, b:=2000000000} (build n l m) (build n (m+1) r)

-- Insert is structurally recursive on Tree
def insert (t : Tree) (newLine : Line) : Tree :=
  match t with
  | Tree.empty => Tree.empty
  | Tree.node l r line left right =>
    let m : Int := ((l : Int) + (r : Int)) / 2
    let val_new := newLine.eval m
    let val_old := line.eval m
    let betterAtMid := val_new < val_old
    
    let keep := if betterAtMid then newLine else line
    let push := if betterAtMid then line else newLine
    
    if l == r then
      Tree.node l r keep left right
    else
      -- Check left endpoint to decide direction
      let val_push_l := push.eval l
      let val_keep_l := keep.eval l
      -- If push is better at left, they must intersect in [l, m], so push goes left
      if val_push_l < val_keep_l then
        Tree.node l r keep (insert left push) right
      else
        Tree.node l r keep left (insert right push)

-- Query is structurally recursive on Tree
def query (t : Tree) (x : Int) : Int :=
  match t with
  | Tree.empty => 2000000000
  | Tree.node l r line left right =>
    let val := line.eval x
    let m := ((l : Int) + (r : Int)) / 2
    if x < l || x > r then 2000000000 
    else if l == r then val
    else
      if x <= m then
        min val (query left x)
      else
        min val (query right x)

def brute_force_min (lines : List Line) (x : Int) : Int :=
  lines.foldl (fun m l => min m (l.eval x)) 2000000000

theorem query_correct (lines : List Line) (x : Int) (l r : Nat) (h_in : l <= x ∧ x <= r) :
  let t_init := build (r - l + 1) l r
  let t_final := lines.foldl insert t_init
  query t_final x = min (query t_init x) (brute_force_min lines x) := by
  sorry

end LiChao
