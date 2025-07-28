# Version Control(Git)
原文链接：https://missing.csail.mit.edu/2020/version-control/
## Git's data model
### Snapshots
git 通过一系列 snapshots 来建模文件和文件夹的历史，在 git 中：
* blob：一个文件，它仅仅是字节的集合。
* tree：一个目录，它包含了 name 与 blobs 和 trees 的映射关系。
* snapshot：被跟踪的 top-level tree。
```
<root> (tree)
|
+- foo (tree)
|  |
|  + bar.txt (blob, contents = "hello world")
|
+- baz.txt (blob, contents = "git is wonderful")
```
在这里， `root` 就是一个 snapshot，它有两个元素：`foo(tree)` 和 `baz.txt(blob)`。
### Modeling history:relating snapshots
版本控制系统需要提供某种建模方式，来将不同的 snapshots 联系起来，从而建模提交历史。比如通过一个线性链来建模提交历史，这样一个提交历史就等价于一个按照时间顺序排列的 snapshots 链。
在 git 中，使用了有向无环图（directed acyclic graph, DAG）来建模提交历史，每一个 snapshot 都是一个节点，它指向它的一系列父节点（在它之前的提交）。一系列父节点比如 merge 的时候，新的节点可能就指向两个父节点，而线性提交历史永远只指向一个父节点。
Git 中的快照就是 commit，在 git 中，commit 是不可以改变了，这意味着修改 commit history 实际上是创建了新的 commit，然后更新 references 指向新的 commit。
### Data model,as pseudocode
```c
// a file is a bunch of bytes
type blob = array<byte>

// a directory contains named files and directories
type tree = man<string, tree | blob>

// a commit has parents, metadata, and the top-level tree
tyep commit = struct{
    parents: array<commit>
    author: string
    message: string
    snapshot: tree
}
```
### Objects and content-addressing
git 中，一个 object 是一个 blob, tree, 或 commit，在存储的时候，所有的对象的地址是它们的 SHA-1 哈希值。
```python
type object = blob | tree | commit
objects = map<string, object>
def store(object):
    id = sha1(object)
    objects[id] = object

def load(id):
    return objects[id]
```
当一个 object 引用其他 objects 的时候，不会再磁盘上包含它的内容，仅仅通过它们的哈希值来引用它们。
### References
所有的 snapshots 都被他们的 SHA-1 哈希值所表示，但是人们并不擅长记忆 40 位长的字符串，所以 git 提供了 `references`，他是一个指向 commits 的指针，与 commits 不同，references 可以改变，它们可以指向任何 commits。
```python
references = map<string, string>

def update_referene(name, id):
    references[name] = id

def read_reference(name):
    return references[name]

def load_reference(name_or_id):
    if name_or_id in references:
        return load(references[name_or_id])
    else:
        return load(name_or_id)
```
在 git 中，有一个特殊的 reference，它叫做 `HEAD`，它指向当前的 commit，从而告诉我们 where we currently are。
### Repositories
git 中的仓库就是：it is the data objects and references.在磁盘中，git 存放的就只是 objects 和 references，所有的 git commands 都会映射中通过 adding objects 和 adding/updating references 来操作 commit DAG.
## Staging area
存放下一次 commit 的内容。
## Git command-line interface
### Basic command
* git help <command>
* git log --all --graph --decorate：visualizes history as a DAG
* git checkout <revision>：updates HEAD(and current branch if checking out a branch)
### Remotes
* git remote add <name> <url>：add a remote
* git push <remote> <local branch>:<remote branch>：send objects to remote, and update remote references
* git branch --set-upstream-to=<remote>/<remote branch>：set up correspondence between local and remote branche
### Undo
* git commit --amend：edit a commit's contents/message
* git reset HEAD <file>：unstage a file
* git checkout -- <file>：discard changes