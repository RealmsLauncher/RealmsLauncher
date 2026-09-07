# Contributing to Realms Launcher

Thank you for taking the time to contribute! Contributions should improve the project, be understandable to reviewers, and respect the licensing and development requirements described below.

## Before you contribute

For larger changes, open an issue or RFC first so the approach can be discussed before significant development work begins.

For smaller fixes:

1. Fork the repository or create a development branch.
2. Make the smallest focused change that solves the problem.
3. Build and test the affected code.
4. Run formatting/linting where applicable.
5. Review the complete diff before opening a pull request.

## Restrictions on generative AI usage

> [!NOTE]
> This policy is adapted from guidance used by other large open-source projects, including matplotlib and the Linux kernel.

We expect authentic engagement in the Realms Launcher community.

- Do not post raw output from large language models or similar generative AI systems as GitHub or Discord comments. Such content is often formulaic and low-value.
- AI tools may be used as development aids, but contributors must understand the changes they submit and be able to explain why the implementation is correct.
- AI-generated code or documentation must be reviewed and meaningfully validated by the human contributor before submission.
- Low-effort changes that amount to copying an AI response without understanding or testing it may be rejected.

### AI attribution

When AI tools materially assist a contribution, include an `Assisted-by` tag in the commit message using this format:

```text
Assisted-by: AGENT_NAME:MODEL_VERSION [TOOL1] [TOOL2]
```

For example:

```text
Assisted-by: Claude:claude-3-opus clang-tidy
```

Basic development tools such as Git, GCC, CMake, editors, and formatters do not need to be listed.

### Signed-off-by and the DCO

**AI agents must not add `Signed-off-by` tags.** Only the human contributor can legally certify the Developer Certificate of Origin (DCO).

The human submitter is responsible for:

- Reviewing all AI-assisted code.
- Ensuring the contribution complies with the project's licenses.
- Adding their own `Signed-off-by` tag.
- Taking full responsibility for the contribution.

## Code style

C++ files are formatted with `clang-format` using the repository's `.clang-format` configuration. Run it on changed files before committing.

The project's general C++ naming conventions are:

- Classes and types: `PascalCase`
- Private/protected data members: `m_camelCase`
- Private/protected static data members: `s_camelCase`
- Public data members: `camelCase`
- Static constants and macros: `SCREAMING_SNAKE_CASE`
- Member functions and global functions: `camelCase`
- Enum constants: `PascalCase`

Avoid inventing unnecessary abbreviations or renaming existing identifiers solely to make them match the conventions. Large mechanical renames make reviews harder and increase merge conflicts.

The repository also contains a `.clang-tidy` configuration for additional static analysis.

## Example style

```cpp
#define AWESOMENESS 10

constexpr double PI = 3.14159;

enum class PizzaToppings { HamAndPineapple, OreoAndKetchup };

struct Person {
    QString name;
    QDateTime dateOfBirth;

    long daysOld() const { return dateOfBirth.daysTo(QDateTime::currentDateTime()); }
};

class ImportantClass {
   public:
    void incrementCounter()
    {
        if (m_counter + 1 > MAX_COUNTER_VALUE)
            throw std::runtime_error("Counter has reached limit!");

        ++m_counter;
    }

    int counter() const { return m_counter; }

   private:
    static constexpr int MAX_COUNTER_VALUE = 100;
    int m_counter;
};
```

## Building and testing

See [`docs/BUILDING.md`](docs/BUILDING.md) for the complete source-build guide.

At minimum, verify the affected configuration builds successfully and run the relevant tests:

```bash
cmake --preset linux
cmake --build --preset linux --config Debug
ctest --preset linux --build-config Debug
```

If you are working on a platform-specific change, test the corresponding CMake preset when possible.

## Pull requests

A good pull request should:

- Explain **what** changed and **why**.
- Keep unrelated changes out of the PR.
- Include tests or a clear explanation when tests are not practical.
- Mention user-visible behavior changes.
- Call out dependency, build-system, or security implications.
- Preserve existing licensing and attribution notices.
- Pass the repository's required checks.

Please do not include secrets, access tokens, personal credentials, or private account data in source code, logs, screenshots, or issue reports.

## Signing your work

All contributions must be signed off with a DCO sign-off. The easiest way is to use `-s` when creating commits:

```bash
git commit -s -m "fix: describe the change"
```

This adds a line like:

```text
Signed-off-by: Your Name <your@email.example>
```

By signing off, you certify the Developer Certificate of Origin 1.1:

```text
Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.
```

The repository's DCO automation will report commits that are missing the required sign-off.

For more information about cryptographically signing commits, see [GitHub's commit-signing documentation](https://docs.github.com/en/authentication/managing-commit-signature-verification/signing-commits).

## Backports

This repository includes an automated backport workflow under [`.github/workflows/backport.yml`](.github/workflows/backport.yml). If a change needs to be maintained on another release branch, use the repository's configured backport labels and follow the workflow conventions.

## Questions

If you are unsure whether a change belongs in a pull request, open an issue or RFC first. For bugs, use the repository's bug-report form so the maintainers receive the information needed to reproduce the problem.
