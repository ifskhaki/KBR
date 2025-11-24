# Contributing to Bitrecover

Thank you for your interest in contributing to Bitrecover! This document provides guidelines and instructions for contributing.

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help others learn and grow

## How to Contribute

### Reporting Bugs

1. Check if the bug has already been reported in Issues
2. Create a new issue with:
   - Clear description
   - Steps to reproduce
   - Expected vs actual behavior
   - System information (OS, GPU, CUDA version)

### Suggesting Features

1. Check existing feature requests
2. Create a new issue with:
   - Use case description
   - Proposed solution
   - Benefits

### Code Contributions

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/your-feature-name`
3. **Make your changes**:
   - Follow existing code style
   - Add comments for complex logic
   - Update documentation if needed
4. **Test your changes**:
   - Build successfully
   - Test on your system
   - Ensure no regressions
5. **Commit your changes**:
   - Use clear, descriptive commit messages
   - Reference issue numbers if applicable
6. **Push to your fork**: `git push origin feature/your-feature-name`
7. **Create a Pull Request**:
   - Describe what changes you made
   - Reference related issues
   - Include test results

## Coding Standards

### C++ Style

- Use C++17 features
- Follow existing code formatting
- Use meaningful variable names
- Add comments for complex algorithms
- Keep functions focused and small

### File Organization

- Headers in `include/bitrecover/`
- Source in `src/`
- One class per file pair (.h/.cpp)

### Commit Messages

Format: `[Type] Brief description`

Types:
- `[Feature]` - New functionality
- `[Fix]` - Bug fix
- `[Docs]` - Documentation
- `[Refactor]` - Code restructuring
- `[Test]` - Tests
- `[Build]` - Build system changes

Example: `[Feature] Add multi-GPU support for RTX 40xx series`

## Testing

- Test on multiple GPU architectures if possible
- Verify email notifications work
- Check status display updates correctly
- Test with various address file sizes

## Documentation

- Update README.md for user-facing changes
- Add code comments for new features
- Update this file if contributing guidelines change

## Questions?

Open an issue with the `question` label or start a discussion.

Thank you for contributing to Bitrecover! 🚀

