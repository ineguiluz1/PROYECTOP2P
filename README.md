# Hybrid Peer-to-Peer (P2P) System
This project aims to develop a hybrid Peer-to-Peer (P2P) system implemented in C and C++. The goal is to create a decentralized file-sharing platform that combines the advantages of pure P2P systems with the efficiency of a centralized index server.

## Project Overview
The P2P concept has revolutionized the way files are shared on the network, allowing users to exchange information without the need for a central server. However, pure P2P systems present challenges in scalability and efficiency due to the complexity of the network. To address these challenges, this project focuses on the design and implementation of a Napster-type hybrid P2P system.

## System Design
In this system, users register with the central server, where they are assigned a unique identifier that distinguishes them within the network. This central server, known as the index server, plays a crucial role in indexing files shared by network users. By registering information about available files and their locations in the network, the central server becomes a massive centralized repository of metadata.

## File Searching and Sharing
When a user wants to search for a specific file, they send a request to the central server. This request contains relevant information about the desired file, such as its name or type. The central server searches the database and returns relevant results to the user, including the IP addresses of users sharing the desired files. The user can then establish a direct connection with other users sharing the desired files.

File sharing between users is done directly, without the intervention of the central server. During this process, the central server does not participate in the actual data transfer. Essentially, its function is to facilitate the initial connection between users.

## Database Updates
As users add or remove shared files, the central server updates its database to reflect these changes. This ensures that indexing information is always available and up-to-date.

## Future Work
This project is a starting point for exploring the potential of hybrid P2P systems. Future work could include optimizing the system for large-scale networks, implementing advanced search algorithms, and enhancing security features.