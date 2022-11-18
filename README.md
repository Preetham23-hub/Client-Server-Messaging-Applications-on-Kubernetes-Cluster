# Client-Server-Messaging-Applications-on-Kubernetes-Cluster

client and server machines communicating and sending messages over Kubernetes cluster.

1. Here I have created two socket client and server machines which can communicate and send packets over the kubernetes cluster, which helps to understand the pod to pod communication inside a cluster.

2. you can log into the running pod and can able to see the messages they are sending and receiving each other, message from application and message count's are visible.

3. client application will auto discover the server machine IP address and port number then starts to send the messages.

4. you can customize packet transfer speed between the application by modifying client-config in deployment yaml file.

5. To visualize the packets communication between machines, log into any one running pod and execute "tcpdump -i eth0" on shell.

Deployment steps

1. To run server messaging application:
   kubectl create -f serverdeployment.yaml

2. To run client messaging application:
   kubectl create -f clientdeployment.yaml

3. To get pods details:
   kubectl get pods

4. To get deployment details:
   kubectl get deployment

5. To check the logs of the pods:
   kubectl logs 'podname'

6. To check tcpdump:
   kubectl exec -it 'podname" sh
   tcpdump -i eth0
