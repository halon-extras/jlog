# JLog plugin

This plugin allows you to log messages to a [jlog](https://github.com/omniti-labs/jlog) message queue (```jlogger(id, data)```).

## Installation

Follow the [instructions](https://docs.halon.io/manual/comp_install.html#installation) in our manual to add our package repository and then run the below command.

### Ubuntu

```
apt-get install halon-extras-jlog
```

### RHEL

```
yum install halon-extras-jlog
```

## Configuration

For the configuration schema, see [jlog.schema.json](jlog.schema.json). Below is a sample configuration.

> **_Important!_**
> 
> If using the `/var/log/halon/` folder as in the sample below and it does not exist, when you create it - give it the same permission as the `smtpd` process is using. Eg.
> ```
> mkdir /var/log/halon
> chown halon:staff /var/log/halon
> ```
> 

### smtpd.yaml

```
plugins:
  - id: jlog
    config:
      queues:
        - id: elastic
          path: /var/log/halon/elastic.jlog
          subscribers:
            - myelasticreader
```

## Example

```
import { jlogger } from "extras://jlog";
jlogger("elastic", json_encode([]));
```

## Autoload

This plugin creates files needed and used by the `smtpd` process, hence this plugin does not autoload when running the `hsh` script interpreter. There are two issues that may occur

1) Bad file permission if logs are created by the user running `hsh` not the `smtpd` process.
2) Mulitple subscribers on the logs file (`smtpd` and `hsh`). Do use this plugin in `hsh` if `smtpd` is running and vice versa.

To overcome the first issue, run `hsh` as `root` and use the `--privdrop` flag to become the same user as `smtpd` is using.

```
hsh --privdrop --plugin jlog
```