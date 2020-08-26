<h3 align="center">
    <img src="https://user-images.githubusercontent.com/30767528/91281665-a8b19400-e788-11ea-8aa3-cd54f00198f6.png" alt="Logo" width="300">
</h3>

<h3 align="center">
    Authenticate incoming OpenVPN connections.
</h3>

<br/>

<p align="center">
  <a href="https://github.com/openvpn-access/authenticator/actions?query=workflow%3ACI"><img
     alt="CI Status"
     src="https://github.com/openvpn-access/authenticator/workflows/CI/badge.svg"/></a>
  <a href="https://tightrope.seymour.global/signup_user_complete/?id=io8xcu5aotg65bmjmoe94supwy" target="_blank"><img 
     alt="Mattermost Developer Chat"
     src="https://img.shields.io/badge/Developer%20chat%20on-mattermost-blue"></a>
  <a href="https://github.com/sponsors/Simonwep"><img
     alt="GitHub Sponsors"
     src="https://img.shields.io/badge/GitHub-sponsor-0A5DFF.svg"></a>
  <a href="https://www.buymeacoffee.com/aVc3krbXQ"><img
     alt="Buy me a Coffee"
     src="https://img.shields.io/badge/Buy%20Me%20A%20Coffee-donate-FF813F.svg"></a>
  <a href="https://www.patreon.com/simonwep"><img
     alt="Support on Patreon"
     src="https://img.shields.io/badge/Patreon-support-FA8676.svg"></a>
</p>

<br>

This project is a core piece of the OpenVPN-Access project. 
It intercepts all incoming OpenVPN connections and verifies the provided usernames and passwords against those stored in a `MariaDB` database.

> This project is part of the [OpenVPN Access](https://github.com/openvpn-access) eco-system.
Be sure to check out the [main repository](https://github.com/openvpn-access/openvpn-access) to get started. 

### Building from Source

OpenVPN-Authenticator uses `cmake` to create its build files and configurations, and depends on the following pieces of software:

* [Googletest](https://github.com/google/googletest) to build and run tests (optional).
* [libbcrypt](https://github.com/rg3/libbcrypt.git) to verify passwords against their hashed versions in the database.
* [mariadbpp](https://github.com/viaduck/mariadbpp.git) to connect to the `MariaDB` database instance.
* [yaml-cpp](https://github.com/jbeder/yaml-cpp.git) to parse `authenticator` configuration files.

These, however, are all pulled from their GitHub repositories at compile time by `cmake`, so there is no need to install them beforehand.

To build OpenVPN-Authenticator, you will need:

* A C++20 compatible compiler. We recommend [`g++`](https://gcc.gnu.org/).
* [`cmake`](https://cmake.org/) to generate build files.
* [`doxygen`](https://www.doxygen.nl/index.html) to generate documentation files (optional).

If you meet these requirements, building OpenVPN-Authenticator is as simple as:

```
mkdir build && cd build
cmake ..
cmake --build .
```

If you have any problems, questions or comments, please don't hesitate to [open an issue](https://github.com/openvpn-access/authenticator/issues/new), or [contact us on mattermost](https://tightrope.seymour.global/signup_user_complete/?id=io8xcu5aotg65bmjmoe94supwy).

### Documentation

OpenVPN-Authenticator is well documented in-source, but for the sake of completion, you can find a full `doxygen` generated HTML documentation site [here](https://openvpn-access.github.io/authenticator/).

### Contributing

Issues, pull requests, etc. are very welcome. We are always looking for more people to help us maintain and improve this project.
If you don't know where to start, or have any questions, please contact us on [mattermost](https://tightrope.seymour.global/signup_user_complete/?id=io8xcu5aotg65bmjmoe94supwy), or send an email to Nathan Seymour <nathan@seymour..
