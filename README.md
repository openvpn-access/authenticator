<h1 align="center">
    <img width="300" src="media/vpnms.png" alt="Logo">
</h1>

<h3 align="center">
    Authenticate incoming OpenVPN connections.
</h3>

<p align="center">
    <a href="https://tightrope.seymour.global/signup_user_complete/?id=io8xcu5aotg65bmjmoe94supwy" target="_blank">
        <img src="https://img.shields.io/badge/Developer%20chat%20on-mattermost-blue" alt="Mattermost Developer Chat">
    </a>
</p>

<br>

### OpenVPN-Authenticator

This project is a core piece of the OpenVPN-Access project. It intercepts all incoming OpenVPN connections and verifies the provided usernames and passwords against those stored in a `MariaDB` database.

### Building from Source

OpenVPN-Authenticator uses `cmake` to create its build files and configurations, and depends on the following pieces of software:

* [Googletest](https://github.com/google/googletest) to build and run tests (optional).
* [libbcrypt](https://github.com/rg3/libbcrypt.git) to verify passwords against their hashed versions in the database.
* [mariadbpp](https://github.com/viaduck/mariadbpp.git) to connect to the `MariaDB` database instance.
* [yaml-cpp](https://github.com/jbeder/yaml-cpp.git) to parse `authenticator` configuration files.

These, however, are all pulled from their GitHub repositories at compile time by `cmake`, so there is no need to install them beforehand.

To build OpenVPN-Authenticator, you will need:

* A C++20 compatible compiler. We reccommend `g++`.
* `cmake` to generate build files.
* `doxygen` to generate documentation files (optional).

If you meet these requirements, building OpenVPN-Authenticator is as simple as:

```
mkdir build && cd build
cmake ..
cmake --build .
```

If you have any problems, questions or comments, please don't hesitate to open an issue, or [contact us on mattermost](https://tightrope.seymour.global/signup_user_complete/?id=io8xcu5aotg65bmjmoe94supwy).
