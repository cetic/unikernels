# Unikernel and Immutable Infrastructures

## Table of Contents

* [Introduction](https://git.cetic.be/stages/unikernels#introduction)
* [Theoretical Concepts](https://git.cetic.be/stages/unikernels#theoretical-concepts)
    * [Unikernel](https://git.cetic.be/stages/unikernels#theoretical-concepts)
        * [Microkernel](https://git.cetic.be/stages/unikernels#microkernel)
            * [Monolithic vs Microkernel](https://git.cetic.be/stages/unikernels#monolithic-vs-microkernel)
        * [Library Operating Systems](https://git.cetic.be/stages/unikernels#library-operating-systems)
        * [Unikernel](https://git.cetic.be/stages/unikernels#unikernel-1)
    * [Hypervisors VS Linux Containers VS Unikernel](https://git.cetic.be/stages/unikernels#hypervisors-vs-linux-containers-vs-unikernel)
    * [Security in Unikernel](https://git.cetic.be/stages/unikernels#security-in-unikernel)
    * [Immutable Infrastructures](https://git.cetic.be/stages/unikernels#immutable-infrastructures)
* [State of the Art](https://git.cetic.be/stages/unikernels#state-of-the-art)
    * [Existing Projects](https://git.cetic.be/stages/unikernels#existing-projects)
        * [MiniOS](https://git.cetic.be/stages/unikernels#minios)
        * [ClickOS](https://git.cetic.be/stages/unikernels#clickos)
        * [HalVM](https://git.cetic.be/stages/unikernels#halvm)
        * [IncludeOS](https://git.cetic.be/stages/unikernels#includeos)
        * [MirageOS](https://git.cetic.be/stages/unikernels#mirageos)
        * [OSv](https://git.cetic.be/stages/unikernels#osv)
        * [Rumprun](https://git.cetic.be/stages/unikernels#rumprun)
        * [Unik](https://git.cetic.be/stages/unikernels#unik)
        * [Clive](https://git.cetic.be/stages/unikernels#clive)
        * [Drawbridge](https://git.cetic.be/stages/unikernels#drawbridge)
        * [GUK](https://git.cetic.be/stages/unikernels#guk)
        * [LING](https://git.cetic.be/stages/unikernels#ling)
        * [Runtime.js](https://git.cetic.be/stages/unikernels#runtimejs)
    * [Comparing Solutions](https://git.cetic.be/stages/unikernels#comparing-solutions)
* [Use Case](https://git.cetic.be/stages/unikernels/#use-case)
* [Proof of Concept](https://git.cetic.be/stages/unikernels#proof-of-concept)
    * [Choice of Unikernel Solution](https://git.cetic.be/stages/unikernels#choice-of-unikernel-solution)
    * [Architecture of the Proof of Concept](https://git.cetic.be/stages/unikernels#architecture-of-the-proof-of-concept)
    * [Creating the Unikernel Proof of Concept](https://git.cetic.be/stages/unikernels#creating-the-unikernel-proof-of-concept)
        * [IncludeOS build files in more details](https://git.cetic.be/stages/unikernels#includeos-build-files-in-more-details)
    * [Creating the Container Analog](https://git.cetic.be/stages/unikernels#creating-the-container-analog)
* [Benchmark & Results](https://git.cetic.be/stages/unikernels#benchmark-results)
    * [Benchmarking Methodology](https://git.cetic.be/stages/unikernels#benchmarking-methodology)
        * [Porting IncludeOS code to Linux Containers](https://git.cetic.be/stages/unikernels#porting-includeos-code-to-linux-containers)
		* [Benchmarking tools used](https://git.cetic.be/stages/unikernels/#benchmarking-tools-used)
	* [Benchmark Results](https://git.cetic.be/stages/unikernels/#benchmark-results)
* [Project's Reproducibility](https://git.cetic.be/stages/unikernels/#projects-reproducibility)
* [Conclusion](https://git.cetic.be/stages/unikernels#conclusion)
* [Bibliography](https://git.cetic.be/stages/unikernels#bibliography)

## Introduction

In our modern 21st century, it is becoming increasingly hard to imagine a world without access to services in the cloud. From contacting someone through mail, to storing work-related documents on an online drive and accessing it across devices, so many services have risen since the dawn of the Internet.

As the need for both compute and electrical power in the cloud is growing, so are the infrastructures. Virtualization has been a huge push towards offering more services with less hardware. By allowing to bypass the limitations of a single operating system per machine, the cloud has become more powerful and more versatile.

However, all this power comes at a cost. While large datacenters are offering services in the cloud, they are also hungry for electric power, which is becoming a growing concern as our planet is being drained of its resources. Is it possible to imagine giving up all the services we’ve grown accustomed to? Falling back to the older, less power-hungry ways?

Fortunately, virtualization is not a dead end, and innovative solutions have risen to aid in solving the power-hunger of large virtualization infrastructures. One such solution has seen the light of day: what if, instead of virtualizing an entire operating system, you were to load an application with only the required components from the operating system? Effectively reducing the size of the virtual machine to its bare minimum resource footprint? This is where unikernels come into play.

## Theoretical Concepts

### Unikernel
Unikernel is a relatively new concept that was first introduced around 2013 by Anil Madhavapeddy in a paper titled “Unikernels: Library Operating Systems for the Cloud” (Madhavapeddy, et al., 2013). Unikernels are defined by the community at Unikernel.org as follows.

*“Unikernels are specialized, single-address-space machine images constructed by using library operating systems.” (Unikernel, n.d.)*

**Specialized** indicates that a unikernel holds a single application. **Single-address space** means that in its core, the unikernel does not have separate user and kernel address space (more on this later). **Library operating systems** are the core of unikernel systems. The following sections will explain these concepts in more details.

Despite their relatively young age, unikernels borrow from age-old concepts rooted in the dawn of the computer era: microkernels and library operating systems.

#### Microkernel

As opposed to monolithic kernels which contain large amounts of code in kernel space, thus making it rather large, microkernels aim at reducing the size of the kernel. By doing so, microkernels diminish the amount of code in kernel space in favor of modules executed in user space.

A second aspect that microkernels aim to solve is reliability. The basis of this notion is that the more code there is, the higher the chance of encountering bugs as well as potential security flaws in the kernel. In keeping the kernel size small, microkernels reduce the risk of bugs and flaws in the kernel, which can prove fatal to a system’s operation.

##### Monolithic vs Microkernel

Nowadays, monolithic kernels are mostly employed to provide a single version of an operating system that can potentially execute any function required. Windows and Linux are prime examples. Since neither Microsoft nor Linus Torvalds (amongst others) know what users are going to do with the operating system, the kernel integrates as much functionalities as possible out of the box (e.g.: communicating on the network, accessing files on a hard disk, launching multiple services, etc.).

In a scenario using the microkernel approach and applying it to widely used operating systems, the operating system in question would be very small. However, each user would have to install various modules based on what they want to do, because the microkernel operating system includes the bare minimum out of the box. Any additional function requires a module that is executed in user space and interacts with the underlying microkernel.

While microkernels are not ideal for lambda users (as opposed to monolithic), they are very useful in areas where reliability is a requirement. Since modules operate in user space separate from the kernel and other modules’ user space, an issue in one module cannot impact another module.

For example, in a monolithic kernel, file management functionalities are directly integrated within the kernel. Thus, if the file management were to crash, the entire kernel could be impacted, resulting in the entire system crashing (e.g.: Microsoft’s Blue Screen of Death). If another application were to run on the same machine, this service would be affected by the crash even if it had no relation to the file management function.

In a microkernel implementation, using the same use case, to access files on the disk, the appropriate module needs to be loaded in the current microkernel operating system. The same goes for providing a service on the network, another module is required and needs to be loaded. However, if the file management module were to crash, being operated in its own user space, the kernel would not be affected, leaving the system up and running. Moreover, the network module would also be untouched, because it is executing in its own user space as well, separate from the file management module.

#### Library Operating Systems

Library Operating System is another method of constructing an operating system where the kernel and modules required by an application run in the same address space as the application itself. As opposed to the microkernel and monolithic kernel, this implies that there is no separation between kernel and user space, and the application has direct access to the kernel level functions without requiring system calls.

The aim of the library OS is to provide enhancement by exposing low-level hardware abstractions. Unfortunately, the trade off to abstracting low-level hardware is the difficulty in supporting a large range of hardware. This means that to create a full library operating system, the kernel must be compiled with device drivers written for the specific destination hardware. This made for very poor portability of library operating systems, and at the time of conceptualization no convenient solution presented themselves to solve such issue.

Nowadays, however, virtualization already provides an abstraction of the underlying hardware by exposing virtualized hardware drivers. This allows library OS implementations to support the generic virtual driver as opposed to attempting to support various hardware. This provides the basis upon which unikernel applications can be built: by combining an already tested technology that is virtualization, with library operating systems loaded with hypervisor drivers for full portability in an orchestrated environment.

#### Unikernel

Unikernels are specialized because they comprise of a single application offering a single service. Unikernels also make use of library operating systems to minimize the kernel size by including only the functions and routines required (as opposed to microkernel which require modules to be loaded, unikernels are compiled with the required modules beforehand). As a result, unikernel applications provide small, lightweight and highly efficient virtualized applications.

If we look at an application on a monolithic operating system, as indicated by Figure 1, we can see that for an application to run, two address spaces need to exist. A kernel space containing functions offered by the underlying operating system such as accessing I/O devices, file systems and process management. On top of the kernel space is the user space, containing the application code itself.

![Application stack on a monolithic operating system](https://git.cetic.be/stages/unikernels/raw/feature/stagelongree2018/MEDIA/normal_application_stack.PNG "Application stack on a monolithic operating system. Source: (Pavlicek, 2017)")

*Figure 1 Application stack on a monolithic operating system. Source: (Pavlicek, 2017)*

The application code in the user space relies on the operating system code in the kernel space to access various functions and the hardware abstracted by the operating system itself. This approach is most efficient when considering an operating system without prior knowledge of what applications are going to be run on it. Therefore, the monolithic kernel becomes bulky in its attempt to accept a wide array of applications.

Unikernel applications, however, present a very different structure. As indicated by Figure 2, an application running in a unikernel does not present any division in its address space, which holds both the high-level application code and the lower level operating system routines.

![Application stack on a unikernel application](https://git.cetic.be/stages/unikernels/raw/feature/stagelongree2018/MEDIA/unikernel_application_stack.PNG "Application stack on a unikernel application. Source: (Pavlicek, 2017)")

*Figure 2 Application stack on a unikernel application. Source: (Pavlicek, 2017)*

To create such an application, unikernels use specialized cross-compiling (because by design, the unikernel cannot be compiled on the same system it will run) methods by selecting required low-level functions from library operating system (provided in compilable form) and cross-compiling them with the application code and configuration. The result is an image that can run as standalone to provide a service.

### Hypervisors VS Linux Containers VS Unikernel

Virtualization of services can be implemented in various ways. One of the most widespread methods today is through hypervisors such as VMware’s ESXi or Linux Foundation’s Xen Project.

Hypervisors, amongst other things, allow hosting multiple guest operating systems on a single physical machine. The widespread use of hypervisors is due to their ability to better distribute and optimize the workload on the physical servers as opposed to legacy infrastructures of one operating system per physical server.

Containers are another method of virtualization, which differentiates from hypervisors by creating virtualized environments and sharing the host’s kernel. This provides a lighter approach to hypervisors which requires each guest to have their copy of the operating system kernel, making a hypervisor-virtualized environment resource heavy in contrast to containers which share parts of the existing operating system.

As aforementioned, unikernels leverage the abstraction of hypervisors in addition to using library operating systems to only include the required kernel routines alongside the application to present the lightest of all three solutions.

![Comparison between hypervisors, Linux Container (in this case Docker) and unikernel](https://git.cetic.be/stages/unikernels/raw/feature/stagelongree2018/MEDIA/vms-containers-unikernels.PNG "Comparison between hypervisors, Linux Container (in this case Docker) and unikernel")

*Figure 3 Comparison between hypervisors, Linux Containers (in this case Docker) and unikernels. Source docker.com, adapted with unikernel version*

Figure 3 above shows the major difference between the three virtualization technologies. Here we can clearly see that virtual machines present a much larger load on the infrastructure as opposed to containers and unikernels.

Additionally, unikernels are in direct “competition” with containers. By providing services in the form of reduced virtual machines, unikernels improve on the container model by its increased security. By sharing the host kernel, containerized applications share the same vulnerabilities as the host operating system. Furthermore, containers do not possess the same level of host/guest isolation as hypervisors/virtual machines, potentially making container breaches more damaging than both virtual machines and unikernels.

### Security in Unikernel

As mentioned previously, unikernel services present a certain advantage in contrast to virtualized and containerized services. This is due to their reduced attack surface and reduced exploitable operating system code. When offering a service over a network, both virtual machine and container solutions are packed with more tools than required by the running application. This increases the attack surface greatly.

Imagine the following scenario: a DNS name resolution service. The only purpose of such service is to capture a name resolution request, search the database and send the reply. In a virtualized or containerized case, the Linux or Windows environment providing a DNS service includes much more than just a network stack and a local database. An attack could be undertaken on the remote access functions, the user authentication mechanisms, a flawed driver, and so on. All these attacks having a common goal, accessing a command line interface to execute malicious code remotely.

Taking the same use case on a unikernel service, the library routines required are limited to accessing the network device. Even the database access can be included in the application code . In contrast to the virtual machine and container, the unikernel does not include unnecessary operating system functions such as device management, remote access, authentication mechanisms or even a command line interface.

The same DNS service is only capable of accepting DNS request and sending DNS responses. A malicious attacker cannot leverage remote access functionalities, authentication exploits or remote code execution, simply because the functions required to do so are not present in the target’s code.

<sup>1</sup> *Note: the database is not hard coded in the application code nonetheless. This is due to the way unikernel applications compile configuration and data files: on compilation time, they are linked in the code and treated as libraries in the application stack. Note that more complex infrastructures could use micro-services as opposed to hardcoding. In the case of a DNS unikernel, the database could be held in a separate unikernel.*

### Immutable Infrastructures

Immutability is the concept of once an application is running, it cannot be modified. When said application needs modifications or updates, rather than apply numerous changes and patches, the application is discarded, and the new version takes its place.

Employing a “destroy and provision” approach gives the major advantage of keeping the application lightweight rather than burdening it with patches over the years. A perfect example to this is the Windows operating system. While the first installation may not be too cumbersome, over the years, countless updates and patches will eventually increase its size, increase the code complexity and possibly introduce news bugs and vulnerabilities.

Figure 4 below illustrates the comparison between updating an application in a mutable infrastructure (Windows in our previous example) as opposed to an immutable infrastructure.

![Updating and application in mutable and immutable infrastructures. Source: (Stella, 2016)](https://git.cetic.be/stages/unikernels/raw/feature/stagelongree2018/MEDIA/mutable-vs-immutable-update.PNG "Updating and application in mutable and immutable infrastructures. Source: (Stella, 2016)")
 
*Figure 4 Updating an application in mutable and immutable infrastructures. Source: (Stella, 2016)*

Immutable infrastructures come fully into effect in the case of unikernel applications. Since unikernels are designed to be developed and deployed, without the possibility to remotely connect to it, unikernels are immutable by design. Furthermore, their fast boot times allows for possibilities of seamless updates without interruption (e.g.: the [IncludeOS Liveupdate functionality](http://www.includeos.org/blog/2017/liveupdate.html)).

## State of the Art

This section introduces the current unikernel projects at the time of this writing (February 2018). After explaining the various projects, a comparison is established to determine the best candidate for a proof-of-concept.

### Existing Projects

#### [MiniOS](https://github.com/mirage/mini-os)

MiniOS is a small operating system kernel that is included with the Xen project hypervisor. Since 2015 it has its own GitHub and has been the root for many unikernel projects because of its included kernel drivers for the Xen hypervisor. Projects based on MiniOS include ClickOS and MirageOS.

#### [ClickOS](http://cnp.neclab.eu/projects/clickos/)

As opposed to most “general pupose” projects, ClickOS is specialized in network devices using Network Function Virtualization (NFV). Presented in 2014 at the USENIX symposium, the project has been active on [GitHub](https://github.com/sysml/clickos) since the year 2000 and is still maintained but has seen very few activity over the last few years.

Maintained by NEC Laboratories Europe, the project offers network middlebox virtual machines with small footprint (5MB in size with 30ms boot time) and high network throughput on 10 Gigabits per second interfaces.

Written in C++, ClickOS virtual machines are constructed based on the Xen Project’s MiniOS and are designed to run on a Xen hypervisor.

#### [HalVM](https://galois.com/project/halvm/)

Released by Galois Inc., HalVM is a port of the Glasgow Haskell Compiler that allows developers to write applications in Haskell and compile them to a unikernel appliance ready for deployment on Xen hypervisor.

Currently maintained on [GitHub](https://github.com/GaloisInc/HaLVM), the project is still active in version 2.0 and is in the process of updating to 2.5 with a refactor planned for 3.0 (although this refactor was planned for 2017 (Wick, HaLVM v3: The Vision, The Plan, 2016)).

#### [IncludeOS](http://www.includeos.org/)

Prototype currently under development (v0.11 as of February 2018) for writing unikernel applications in C++. Unlike most projects, Include OS is designed to run on KVM, VirtualBox and VMWare with additional support for cloud providers such as Google Compute Engine, OpenStack and VMWare vCloud.

While IncludeOS is still in prototype and maintained on [GitHub](https://github.com/hioa-cs/IncludeOS), the project already proposes an orchestration tool called [Mothership](http://www.includeos.com/mothership/index.html) for configuring, deploying and managing IncludeOS applications.

#### [MirageOS](https://mirage.io/)

One of the first projects on unikernels, MirageOS is a library operating system containing various libraries used to build unikernel applications. Applications are written in OCaml, development is done using libraries that are transformed to kernel routines when compiled to a unikernel image. Images generated with MirageOS are deployable to Xen hypervisors. The project is still maintained on [GitHub](https://github.com/mirage/mirage), with version 3.0 released in February 2017.

#### [OSv](http://osv.io/)

OSv is particular; it allows porting various applications written in various languages to a unikernel by integrating some kernel functions built-in. Therefore, it is not as light as a fully compiled unikernel, however it provides support for many languages including Java, C, C++, Node and Ruby. It can also be run on a variety of platforms such as VirtualBox, VMWare ESXi, KVM, Amazon EC2 and Google Compute Engine. Virtual appliances with widely used applications are also directly available (Memcached, redis and Apache Cassandra).

Originally developed by a Cloudius Systems, the company changed to ScyllaDB and has shifted focus away from OSv. The project is still active but mainly supported by the community on [GitHub](https://github.com/cloudius-systems/osv).

#### [Rumprun](https://github.com/rumpkernel/rumprun)

Rumprun is an implementation of the Rump kernel<sup>2</sup> that allows to transform most of POSIX-compatible applications into unikernel variants. The NetBSD inspiration of the Rump kernel makes it possible to port many applications to unikernel without requiring large amounts of modifications to the application code itself.

Rumprun also supports applications written in other languages such as C, C++, Erlang, Go, Java, Javascript (node.js), Python, Ruby and Rust and can run on both Xen and KVM. The project is still maintained on [GitHub](https://github.com/rumpkernel/rumprun).

<sup>2</sup> *Note: Rump kernels are composed of drivers designed to match closely the originals without requiring rewriting to accommodate heterogeneous operating systems. The rump kernel was designed to be minimal but highly portable.*

#### [Unik](https://github.com/solo-io/unik)

Unik is not a unikernel nor a library operating system per se, but rather a partial orchestration tool. Written in the Go language, it is a toolbox that allows compilation of application code as well as running and managing unikernels across various cloud providers.

Unik makes use of unikernels and can compile applications into unikernel images using rumprun, OSv, IncludeOS and MirageOS. Additionally, Unik can run and manage these unikernels locally on VirtualBox or full-fledged hypervisors such as Xen, KVM or vSphere, as well as on the cloud on Amazon AWS, Google Cloud and OpenStack.

Unik also exposes a REST API in order to allow integration with orchestration tools such as Kubernetes or Cloud Foundry.

#### [Clive](https://lsub.org/ls/clive.html)

Developed by the LSUB research team at Universidad Rey Juan Carlos of Madrid, Clive is announced as “an operating system designed to work in distributed and cloud computing environments” (Unikernels - Rethinking Cloud Infrastructure, n.d.). Written in Go and compiled with a custom compiler, there are few examples of use cases of implementation and the [GitHub](https://github.com/fjballest/clive) has been inactive for close to two years. No official statement has been given regarding the maintaining of the project.

#### [Drawbridge](https://www.microsoft.com/en-us/research/project/drawbridge/)

Drawbridge was a Microsoft Research project that aimed to modify the Windows 7 kernel structure in order to reduce the number of system calls to the kernel. This was done by “refactoring” and moving routines typically present in the kernel space to the user space. This was combined with process isolation called “picoprocess”.

However, while this approach allows lighter application processes, it still heavily relies on the presence of a kernel in a “hypervisor-like” structure. Furthermore, the kernel functions moved to user space remain intact and include routines that may not be used by the application. As a result, while drawbridge is a loose implementation of library operating systems, they greatly differ from unikernels in effect.

Very little to no information is available concerning how to implement it into a Windows operating system.

#### [GUK](https://github.com/Leonidas-from-XIV/guestvm-guk)

GUK (Project Guest VM Microkernel) was an enhanced version of the minimalistic MiniOS operating system which allows to construct unikernel like systems by loading the required modules onto the microkernel. The project has been inactive since its fork in 2011.

#### [LING](http://erlangonxen.org/)

The product of the Erlang-on-Xen project which aimed at creating applications written in Erlang on the Xen hypervisor. The project site has several use cases where unikernel projects could be implemented as well as demos and stats of LING’s website running on a unikernel.

Unfortunately, the LING repository on [GitHub](https://github.com/cloudozer/ling) has not been updated since 2015 (as of February 2018).

#### [Runtime.js](http://runtimejs.org/)

Runtime.js is a library operating system designed to run JavaScript applications with compatibility with the Node.js API. The only hypervisor supported is KVM.

As of October 2017, the project is no longer maintained and is not ready for production in its current state ([GitHub](https://github.com/runtimejs/runtime)).

### Comparing Solutions

Out of the various existing projects, some standout due to their wide range of supported languages. Out of the active projects, the following table resumes the language they support, the hypervisors they can run on and remarks concerning their functionality.

| Unikernel | Language | Targets | Functions |
| --- | --- | --- | --- |
| ClickOS | C++ | Xen | Network Function Virtualization |
| HalVM | Haskell | Xen |  |
| IncludeOS | C++ | KVM, VirtualBox, ESXi, Google Cloud, OpenStack | Orchestration tool available |
| MirageOS | OCaml | Xen |  |
| OSv | Java, C, C++, Node, Ruby | VirtualBox, ESXi, KVM, Amazon EC2, Google Cloud | Cloud and IoT (ARM) |
| Rumprun | C, C++, Erlan, Go, Java, JavaScript, Node.js, Python, Ruby, Rust | Xen, KVM |  |
| Unik | Go, Node.js, Java, C, C++, Python, OCaml | VirtualBox, ESXi, KVM, XEN, Amazon EC2, Google Cloud, OpenStack, PhotonController | Unikernel compiler toolbox with orchestration possible through Kubernetes and Cloud Foundry |

## Use Case

After researching the existing solutions from which to create unikernel, the next step of the project was to evaluate its capacities in a production environment. This would allow gaining more insight into the life cycle of developing a unikernel for specific applications.

The unikernel environment will be created through a proof of concept. As will be defined later, the proof of concept environment will be virtualized on a Linux operating system with the QEMU hypervisor.

The evaluation of the unikernels shall be done against their closest homologues in terms of service minimization, by deploying the same architecture as the unikernels’ proof of concept using container technologies.

Once both infrastructures have been developed and are stable, a benchmark will be executed on both the unikernel and the container environment. Through this benchmark we shall evaluate elements such as boot times, processor and memory usage, local storage access and potentially load stress testing. Unfortunately, penetration testing will not be covered in this benchmark due to the complexities of evaluation and the time constraints of the project.

## Proof of Concept

As highlighted by the table in [Comparing Solutions](https://git.cetic.be/stages/unikernels#comparing-solutions), numerous options are available depending on the programming language used for an application as well as the desired supporting platforms. Unfortunately, not all platforms are supported for all languages.

### Choice of Unikernel Solution

Our initial choice for deploying a proof of concept was to use Unik for its wide range of supported platform, including OpenStack. Additionally, its ability to directly integrate with the Kubernetes orchestration tool made it an ideal candidate. However early testing of the solution proved it to be non-functional when attempting to compile and execute example unikernel applications provided by the repository. Due to the lack of reactivity from the maintainers of the project regarding recent issues, including issues encountered during our initial test, we had to discard Unik as a potential candidate.

Our second choice shifted to IncludeOS for similar reasons: supported platforms including OpenStack, and its orchestration tool possibilities. One of the main drawback is that it only supports the C++ language (as opposed to Unik’s broader list of languages). While this should not prove to be an issue for our proof of concept, however it does reduce its scope of potential interested parties willing to create unikernel applications to developers proficient in C++.

Another potential candidate we envisioned for this proof of concepts was MirageOS. This was mainly due to its amount of documentation and the activity present on the project’s Git repository. Unfortunately, while the OCaml language was one of its drawback, the most prominent drawback was the supported platform being limited to Xen hypervisors.

Lastly, the remaining potential candidates were OSv and Rumprun for their large selections of supported languages and, in the case of OSv, the supported platforms. However, both options were kept as last resort solutions mainly due to their technology, as they do not entirely behave as unikernels. Both employ kernels larger than needed by including functions unused by the compiled application code.

### Architecture of the Proof of Concept

To best compare the advantages of unikernels to alternatives like Linux Containers, we opted to implement a selection of services to be able to evaluate them in different areas of effect.

As such, we will create multiple unikernel in a topology as follows:
* A REST Web Server offering a simple web page;
* A DNS server, resolving the address of the web server;
* A router connecting the external network to the Web and DNS servers;
* A firewall filtering the incoming packets to the router, blocking any ICMP Echo request/reply messages.

Both the web server and the DNS will be in a DMZ like network, with a router connecting it to the external network and the firewall filtering incoming connections to the router. Figure 5 below represents the logical topology that will be deployed in a hypervisor environment.

![Representation of the Proof of Concept topology](https://git.cetic.be/stages/unikernels/raw/feature/stagelongree2018/MEDIA/PoC%20Topology.png "Representation of the Proof of Concept topology")
 
*Figure 5 Representation of the Proof of Concept topology.*

All unikernels will be executed on a Linux based machine in a QEMU/KVM hypervisor. QEMU is chosen as the hypervisor because it is the default hypervisor used by IncludeOS, but also because of the current limitations in the IncludeOS network stack drivers which causes issues when trying to execute unikernels in hypervisors like VMware Workstation or VirtualBox.

### Creating the Unikernel Proof of Concept

To create unikernels using IncludeOS, the first step is to install the IncludeOS binaries. This is straightforward, following the instructions from the Github project page, clone the repository onto a local Linux machine and install using the provided script. Some variables can be exported globally to manipulate where the binaries should install.

With IncludeOS installed, a hypervisor environment is required to run the unikernel containers. It is important to note that IncludeOS installs QEMU during its own installation because it is the default hypervisor.

In the case of our current proof of concept, our main task to prepare the hypervisor environment is to setup the various virtual networks that will be used by the unikernel virtual machines. Once the virtualized environment is prepared, the unikernels can be developed. IncludeOS provides a sample skeleton directory structure containing the base required files for compiling a unikernel. 

To build a unikernel, IncludeOS makes use of the **cmake** command to create build files from the configurations files in the skeleton directory. These files specify various project properties, but most importantly link the project to the specified libraries for the C++ code as well as specify the desired drivers to integrate into the unikernel image. Once the build files have been created, the **make** command is used to compile the unikernel into an **image file**, which can then be booted on using the QEMU hypervisor.

#### IncludeOS build files in more details

The files used by the **cmake** command are very important in the construction of the IncludeOS unikernel. Following is a list of files that need to be configured accordingly in our proof of concept depending on the unikernel image to build.

**CMakeLists.txt** indicates to cmake some project variables such as the project name, the files containing the C++ code to be linked with which OS libraries to form a bootable image as well as the drivers to be included in the OS. Additionally, it also imports IncludeOS routines for building the unikernel image. This file must be carefully reviewed depending on what OS devices the application needs to access and the destination hypervisor. For example, in the case of the web server, we need to access the network device, thus we need to specify the network driver, which can be “virtionet” if we use QEMU/KVM/VirtualBox or “vmxnet3” for VMware.

**Config.json** holds the configuration for the operating system such as the network device configuration, vlans, routes to implement in case of a router, terminal interfaces to include and so on. While little documentation exists on what those files can contain, IncludeOS also includes another possibility of configuring the operating system through a custom script written in what they call NaCl . These custom scripts can be used instead of the config.json file with the same syntax.

The **dependencies.cmake** is not a required file, but it can be used to gather external dependencies such as external git repositories to clone when compiling. In one of the examples present in the IncludeOS repository, this file clones an external C++ library (developed by the same people at IncludeOS).

The **vm.json** is not required either and is used mainly for QEMU parameters when using the IncludeOS boot command which executes QEMU in the background to boot on a unikernel image.

**Service.cpp** contains the entry point to the unikernel application. This file must contain a **void Service::start() {}** method as the starting point of the application. The libraries to use in the C++ code destined for an IncludeOS unikernel should be those provided by the project. While all the code can be contained in a single file, it can be split into different files as well, however it is important to notify cmake which files to include in the CMakeLists.txt file.

### Creating the Container Analog

*This section will contain the various choices and the steps taken to create the container version of the proof of concept used for evaluation and comparison of the unikernel solutions.*

## Benchmarking & Results

### Benchmarking Methodology

Once our unikernel proof of concept environment is up and running, we shall compare them to their most comparable solutions, Linux Containers. Since both technologies advertise providing services with a reduced footprint, comparing both solution should provide valuable insight into both technologies.

To compare unikernels and containers, the most important aspect is to have a baseline for both applications. It goes without saying that a unikernel, an image with the bare minimum required to run an application, is lighter and faster than a container with an Ubuntu as its base image, thus the importance of a baseline.

Our first goal for this baseline is to maintain the code as close to identical possible between the two services. If the unikernel application executes tasks in one way, and the container executes them in another, the benchmark could be potentially flawed.

Our second goal is to maintain image size to a minimum. As previously stated, benchmarking a unikernel with full-fledged operating system provides little useful data. Therefore, it is important to aim for the same goal on both services by attempting to make the services as small and as light as possible.

#### Porting IncludeOS code to Linux Containers

One of the main difficulties in preparing a baseline for the benchmark is porting the IncludeOS C++ code to a Container equivalent. The main reason being that since the IncludeOS code is written for a custom-built kernel, all the APIs available in IncludeOS are different from the standard Linux C++ APIs. Since unikernels function in a single address space, system calls are not implemented in the same manner in the IncludeOS C++ libraries as they are in the Linux C++ libraries.

Our possibilities for porting the code differs with varying degrees of complexity:
* Compiling the IncludeOS C++ code using the IncludeOS headers in a container for execution.
    * *This approach would allow to keep the code identical to the original, however most headers were written for a different kernel and could potentially not function on execution.*
* Rewriting the code in a C++ using Linux headers for a specific target container’s base image.
    * *This method allows to ensure compatibility with the container, however creates complexity in the need to adapt the code to be as identical as possible to the IncludeOS code.*
    * *On the other hand, this method more closely resembles a real-life scenario, where a developer would code differently based on the target platform.*
* Using the IncludeOS Docker image to run the IncludeOS code in container.
    * *This is the least interesting method of porting because the IncludeOS container executes QEMU inside the container, effectively making it a nested virtualization, which is outside the scope of the benchmark (i.e.: using a comparable baseline).*
	
#### Benchmarking tools used

*This section will define the tools used for benchmarking the various elements we defined in the use case.*

### Benchmark Results

*This section will contain the results of the of the various benchmarks conducted using the tools mentioned in the methodology.*

## Project’s Reproducibility

*This section will detail the steps taken to allow this project to be reproduced by anyone who wishes to test the unikernel system itself. This will include the how the project was rendered reproducible and how it can be reproduced.*

## Conclusion

*This conclusion will be written once the proof of concept has been deployed and the benchmark has been executed. The conclusion will also discuss problems encountered during deployment and final thoughts on the technology of unikernels.*

## Bibliography

- Annoucing Release of GUK (Project Guest VM Microkernel). (2009, June 2). Retrieved February 2018, from Xen Project Blog: https://blog.xenproject.org/2009/06/02/annoucing-release-of-guk-project-guest-vm-microkernel/
- Ballesteros, F. (2014). The Clive Operating System. 
- Ballesteros, F. (n.d.). Clive. Retrieved February 2018, from GitHub: https://github.com/fjballest/clive
- Bratterud, A., Happe, A., & Duncan, B. (2017). Enhancing Cloud Security and Privacy: The Unikernel Solution. Eighth International Conference on Cloud Computing, GRIDs, and Virtualization. Athens, Greece. Retrieved February 12, 2018
- Bratterud, A., Walla, A.-A., Haugerud, H., Engelstad, P., & Begnum, K. (2015). IncludeOS: A minimal, resource efficient unikernel for cloud services. Cloud Computing Technology and Science (CloudCom), 2015 IEEE 7th International Conference (pp. 250-257). IEEE. Retrieved February 12, 2018
- Clive. (n.d.). Retrieved February 2018, from Laboratorio de Sistemas: http://lsub.org/ls/clive.html
- Deploy Network Function Virtualization with IncludeOS Mothership. (n.d.). Retrieved February 2018, from IncludeOS: http://www.includeos.com/mothership/index.html
- Drawbridge. (n.d.). Retrieved February 2018, from Microsoft Research: https://www.microsoft.com/en-us/research/project/drawbridge/
- Erlang on Xen. (n.d.). Retrieved February 2018, from Erlang on Xen: http://erlangonxen.org/
- Galois Inc. (2016, December 4). Home - GaloisInc/HaLVM Wiki. Retrieved February 2018, from GitHub: https://github.com/GaloisInc/HaLVM/wiki
- Galois Inc. (n.d.). The Haskell Lightweight Virtual Machine (HaLVM). Retrieved February 2018, from GitHub: https://github.com/GaloisInc/HaLVM
- HiOA Dept. of Computer Science. (n.d.). IncludeOS. Retrieved February 2018, from GitHub: https://github.com/hioa-cs/IncludeOS
- Hunt, G., Olinsky, R., & Howell, J. (2011, October 17). Drawbridge: A new form of virtualization for application sandboxing. (C. Torre, Interviewer) Retrieved February 2018, from https://channel9.msdn.com//shows//going+Deep//drawbridge-An-Experimental-Library-Operating-System
- IncludeOS. (n.d.). Retrieved February 2018, from IncludeOS: http://www.includeos.org/
- Jordan, M. (n.d.). GUK. Retrieved February 2018, from GitHub: https://github.com/Leonidas-from-XIV/guestvm-guk
- Kantee, A. (2016). The Design and Implementation of the Anykernel and Rump Kernels. Retrieved February 2018, from http://www.fixup.fi/misc/rumpkernel-book/
- Kohler, E. (n.d.). The Click Modular Router. Retrieved February 2018, from GitHub: https://github.com/kohler/click
- Levine, I. (2016, May 6). UniK: Build and Run Unikernels with Ease. Retrieved February 2018, from solo.io: https://www.solo.io/single-post/2017/05/14/UniK-Build-and-Run-Unikernels-with-Ease
- ling: Erlang on Xen. (n.d.). Retrieved February 2018, from GitHub: https://github.com/cloudozer/ling
- Madhavapeddy, A., & Scott, D. (2013). Unikernels: Rise of the virtual library operating system. ACM Queue, 11(11), 30. Retrieved February 12, 2018
- Madhavapeddy, A., Mortier, R., Rotsos, C., Scott, D., Singh, B., Gazagnaire, T., . . . Crowcroft, J. (2013). Unikernels: Library Operating Systems for the Cloud. Retrieved February 6, 2018
- Martins, J., Ahmed, M., Raiciu, C., Olteanu, V., Honda, M., Bifulco, R., & Huici, F. (2014). ClickOS and the Art of Network Function Virtualization. Proceedings of the 11th USENIX Conference on Networked Systems Design and Implementation, (pp. 459-473). Retrieved February 2018
- Microkernels. (2015). In A. Tanenbaum, & H. Bos, Modern Operating Systems Fourth Edition (4th ed., pp. 65-68). Pearson Education, Inc.
- Mini-OS. (2018, February 7). Retrieved February 19, 2018, from Xen Project Wiki: https://wiki.xenproject.org/wiki/Mini-OS
- mirage. (n.d.). Retrieved February 2018, from GitHub: https://github.com/mirage/mirage
- MirageOS. (n.d.). Retrieved February 2018, from MirageOS: https://mirage.io/
- osv. (n.d.). Retrieved February 2018, from GitHub: https://github.com/cloudius-systems/osv
- OSv - the operating system designed for the cloud. (n.d.). Retrieved February 2018, from OSv: http://osv.io/
- Overview. (n.d.). Retrieved February 2018, from ClickOS - Systems and Machine Learning: http://cnp.neclab.eu/projects/clickos/
- Pavlicek, R. (2017). Unikernels: Beyond Containers to the Next Generation of Cloud. O’Reilly Media.
- perbu. (2017, October 15). Introducing Liveupdate. Retrieved February 2018, from includeOS: http://www.includeos.org/blog/2017/liveupdate.html
- Popuri, S. (2018, February 7). Mini-OS-DevNotes. Retrieved February 19, 2018, from Xen Project Wiki: https://wiki.xenproject.org/wiki/Mini-OS-DevNotes
- Porter, D., Boyd-Wickizer, S., Howell, J., Olinsky, R., & Hunt, G. (2011, March). Rethinking the Library OS from the Top Down. ACM SIGPLAN Notices, 46(3), pp. 291-304. Retrieved February 2018
- Rump Kernels. (n.d.). Retrieved February 2018, from Rump Kernels: http://rumpkernel.org/
- runtime.js - JavaScript Library Operating System for the Cloud. (n.d.). Retrieved February 2018, from runtime.js: http://runtimejs.org/
- runtimejs/runtime. (n.d.). Retrieved February 2018, from GitHub: https://github.com/runtimejs/runtime
- solo-io/unik: The Unikernel Compilation and Deployment Platform. (n.d.). Retrieved February 2018, from GitHub: https://github.com/solo-io/unik
- Stella, J. (2016). Immutable Infrastructures. O,Reilly Media, Inc. Retrieved February 2018
- The Rumprun unikernel and toolchain for various platforms. (n.d.). Retrieved February 2018, from GitHub: https://github.com/rumpkernel/rumprun
- Unikernels - Rethinking Cloud Infrastructure. (n.d.). Retrieved February 6, 2018, from Unikernel: http://unikernel.org/
- Unikernels. (2016, October 15). Retrieved February 12, 2018, from Xen Project: https://wiki.xenproject.org/wiki/Unikernels
- Wick, A. (2016, April 29). HaLVM v3: The Vision, The Plan. Retrieved February 2018, from http://uhsure.com/halvm3.html
- Wick, A. (n.d.). HaLVM. Retrieved February 2018, from galois: https://galois.com/project/halvm/
- Wiki for rump kernels. (n.d.). Retrieved February 2018, from GitHub: https://github.com/rumpkernel/wiki