# Security Policy

## Supported Versions

The following table lists the O-RAN Module for ns-3 versions and their current security support status:

| Version  | Supported | Support Status         | Notes                                  |
|----------|:---------:|-----------------------|----------------------------------------|
| 1.2.x    | ✅        | Actively Supported    | Receives all security updates          |
| 1.1.x    | ✅        | Supported             | Receives critical security updates     |
| 1.0.x    | ❌        | End of Support        | No longer receives security updates    |
| < 1.0    | ❌        | Unsupported           | Upgrade required for security coverage |

> **Note:** Users are strongly encouraged to upgrade to the latest supported version to ensure continued security coverage.

## Reporting a Vulnerability

The NIST team takes security vulnerabilities seriously. If you discover a security vulnerability in the O-RAN Module for ns-3, please report it responsibly by following these guidelines:

### Reporting Process

1. **Do not create a public GitHub issue** for security vulnerabilities
2. Send a detailed report to the development team through appropriate channels
3. Include the following information in your report:
   - Description of the vulnerability
   - Steps to reproduce the issue
   - Potential impact assessment
   - Suggested mitigation or fix (if known)

### What to Include

Your security report should contain:

- **Vulnerability Type**: Buffer overflow, injection, privilege escalation, etc.
- **Affected Components**: Specific classes, methods, or modules
- **Attack Vector**: How the vulnerability could be exploited
- **Impact Assessment**: Confidentiality, integrity, or availability impacts
- **Proof of Concept**: Code or steps demonstrating the vulnerability
- **Environment Details**: ns-3 version, OS, compiler information

### Response Timeline

- **Initial Response**: Within 48 hours of receiving the report
- **Vulnerability Assessment**: Within 1 week
- **Fix Development**: 2-4 weeks depending on complexity
- **Security Advisory**: Published with the fix release

### Disclosure Policy

- We follow responsible disclosure practices
- Security fixes are prioritized and released as soon as possible
- Public disclosure occurs only after fixes are available
- Credit is given to security researchers (unless they prefer anonymity)

## Security Best Practices

When using the O-RAN Module for ns-3:

### For Developers

- **Input Validation**: Always validate input parameters in public APIs
- **Memory Management**: Use smart pointers and RAII patterns
- **Database Security**: Use parameterized queries to prevent injection
- **Error Handling**: Avoid information leakage in error messages
- **Code Review**: Implement security-focused code reviews

### For Users

- **Keep Updated**: Use the latest stable version with security patches
- **Secure Configuration**: Follow security guidelines in documentation
- **Access Control**: Implement appropriate file and database permissions
- **Monitoring**: Monitor simulation logs for unusual behavior
- **Network Security**: Secure any network interfaces if applicable

## Known Security Considerations

### SQLite Database

- Database files may contain sensitive simulation data
- Implement appropriate file permissions and access controls
- Consider encryption for sensitive research data

### Machine Learning Models

- Validate ML model files before loading
- Ensure model files come from trusted sources
- Monitor resource usage during model execution

### Memory Management

- Large simulations may consume significant memory
- Implement appropriate resource limits
- Monitor for memory leaks in long-running simulations

## Security Updates

Security updates are distributed through:

- GitHub releases with security tags
- Security advisories in the GitHub repository
- Updates to this security policy as needed

## Contact Information

For security-related questions or concerns:

- Review the contributing guidelines
- Check existing security advisories
- Follow responsible disclosure practices

## Compliance

This security policy aligns with:

- NIST Cybersecurity Framework
- Open source security best practices
- Academic research security guidelines

---

**Note**: This software is provided by NIST as-is. Users are responsible for implementing appropriate security measures in their research environments.
