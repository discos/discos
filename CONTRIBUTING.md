# How to Contribute

We'd love to accept any contribution from anybody. If you like to contribute, please take time to 
read this page.

## Contributor License Agreement

Contributions to this project implicitly adhere to this [agreement](https://discos.readthedocs.io/en/latest/license.html).

## Code reviews

All submissions, including submissions by project members, require review. We
use GitHub pull requests for this purpose. Consult our
[How-to](https://discos.readthedocs.io/en/latest/developer/howto/github/branch_and_merge.html#generating-a-pull-request) for more
information on using pull requests.

## How to file a bug report.

All issues are to be submitted through the GitHub tracking system.

When filing an issue, make sure to answer these five questions:

> 1. What version of DISCOS are you referring? Which telescope are you at?
> 2. What did you do?
> 3. What did you expect to see?
> 4. What did you see instead?

## How to suggest a feature or enhancement.

If you find yourself wishing for a feature that doesn't exist, you are probably not alone. 
Open an issue on our issues list on GitHub which describes the feature you would like to see, 
why you need it, and how it should work.

## Our workflow with GitHub

Our GitHub-based [workflow](https://discos.readthedocs.io/en/latest/developer/howto/github/index.html)  is very simple. The master branch is dedicated to implement new features and capabilities.
The stable branch will be used for software releses according our [policy](https://discos.readthedocs.io/en/latest/developer/releasing.html)

### Hot-Fixes workflow

1.Checked out into master branch
>git checkout master
2.Fetched all remote updates
>git remote update
3.Update local master branch with remote copy
>git pull origin master
Checked out into stable branch
git checkout stable
Update local stable branch with remote copy
git pull origin stable
Created a hotfix branch related to the issue xxx that tracks origin/master
git checkout -b fix-issue-xxx origin/master
Did some fixes and committed to it
Pushed hotfix branch to remote repository
git push origin fix-issue-xxx
Opened a "pull request" in GitHub for team to verify the hotfix
Checkout into stable branch
git checkout stable
Merged hotfix branch into stable branch
git merge fix-issue-xxx
Tagged the hotfix point by creating a new tag
git tag -a 0.1.1 -m 'Create hotfix tag 0.1.1'
Pushed stable branch to remote repository
git push origin stable
Pushed the tags to remote repository
git push origin --tags
Checkout into master branch
git checkout master
Merged hotfix branch into master branch
git merge fix-issue-xxx
Pushed develop branch to remote repository
git push origin master
Removed hotfix branch from the local repository
git branch -D fix-issue-xxx
Removed hotfix branch from the remote repository
git push origin :fix-issue-xxx

### feature workflow

Checked out into master branch
git checkout master
Fetched all remote updates
git remote update
Update local master branch with remote copy
git pull origin master
Created a branch related to the issue xxx that tracks origin/master
git checkout -b fix-issue-xxx origin/master
Did some coding and committed to it
git commit -m "fix #xxx:…"
Pushed feature branch to remote repository
git push origin fix-issue-xxx
Opened a "pull request" in GitHub for team to verify the feature
Checkout into stable master
git checkout master
Merged branch into master branch
git merge fix-issue-xxx
Pushed master branch to remote repository
git push origin master
Removed issue branch from the local repository
git branch -D fix-issue-xxx
Removed isue branch from the remote repository
git push origin :fix-issue-xxx


