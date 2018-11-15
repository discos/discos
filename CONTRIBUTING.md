# How to Contribute

We'd love to accept any contribution from anybody. If you like to contribute, please take time to read this page.

## Contributor License Agreement

Contributions to this project implicitly adhere to this [agreement](https://discos.readthedocs.io/en/latest/license.html).

## Code reviews

All submissions, including submissions by project members, require review. We use GitHub pull requests for this purpose. Consult our [How-to](https://discos.readthedocs.io/en/latest/developer/howto/github/branch_and_merge.html#generating-a-pull-request) for more information on using pull requests.

## How to file a bug report

All issues are to be submitted through the GitHub tracking system.

When filing an issue, make sure to answer the following questions:

> 1. What version of DISCOS are you referring? Which telescope are you at?
> 2. What did you do?
> 3. What did you expect to see?
> 4. What did you see instead?

## How to suggest a feature or enhancement

If you find yourself wishing for a feature that doesn't exist, you are probably not alone. Open an issue on our issues list on GitHub which describes the feature you would like to see, why you need it, and how it should work.

## Our workflow with GitHub

Our GitHub-based [workflow](https://discos.readthedocs.io/en/latest/developer/howto/github/index.html)  is very simple. The master branch is dedicated to implement new features and capabilities. The stable branch will be used for software releases according to our [policy](https://discos.readthedocs.io/en/latest/developer/releasing.html).

### Hot-Fixes workflow

- Check out into master branch
`git checkout master`
- Fetch all remote updates
`git remote update`
- Update local master branch with remote copy
`git pull origin master`
- Check out into stable branch
`git checkout stable`
- Update local stable branch with remote copy
`git pull origin stable`
- Create a hotfix branch related to the issue xxx
`git checkout -b fix-issue-xxx`
- Do some fixes and commit to them
- Push the hotfix branch to remote repository
`git push origin fix-issue-xxx`
- Open a "Pull request" in GitHub, in order to merge the hotfix branch onto the stable branch, and reqeust a review from the team
- Once the team approves the pull request, the branch can be merged using the "Squash and merge" strategy, be sure to not delete the hotfix branch
- Open a second "Pull request" in GitHub, in order to merge the hotfix branch onto the master branch
- Once again, once the team approves the pull request, the branch can be merged using the "Squash and merge" strategy, this time the branch can be deleted
- If some conflict between the master and the hotfix branch arises, don't worry, they can be fixed right away when merging online. Resolve the conflict by porting the new lines from the hotfix branch into the master branch version of the file.
- Finally, delete the hotfix branch from the local repository
`git branch -D fix-issue-xxx`
- Now that the hotfix is on the online repository, a new tag can be created onto the stable branch. In order to do this, go to the releases page of the repository, and select "Draft a new release". Fill the "Tag version" and "Release title" fields according to the previous releases. Also, be sure to select the stable branch as target and to check "This is a pre-release" if you are not sure if your release is ready to be deployed on production line.

### Feature implementation workflow

- Check out into master branch
`git checkout master`
- Fetch all remote updates
`git remote update`
- Update local master branch with remote copy
`git pull origin master`
- Create a branch related to the issue xxx
`git checkout -b fix-issue-xxx`
- Do some coding and commit to it
`git commit -m "Fix #xxx:…"`
- Push feature branch to remote repository
`git push origin fix-issue-xxx`
- Open a "Pull request" in GitHub for team to verify the feature
- Once the team approves the pull request, the branch can be merged using the "Squash and merge" strategy, and it can be deleted from the online repository
- Finally, delete the feature branch from the local repository
`git branch -D fix-issue-xxx`

