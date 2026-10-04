FROM node:18-alpine

WORKDIR /app

# Copy package info
COPY package*.json ./
COPY tsconfig.json ./

# Install dependencies
RUN npm install

# Copy source code
COPY src ./src

# Expose the API port
EXPOSE 3000

# Run development server (ts-node-dev or nodemon in real prod this would be built)
CMD ["npx", "ts-node", "src/server.ts"]
